"""Build and audit the explicit source distribution; never copy a checkout wholesale."""
from pathlib import Path, PurePosixPath
import argparse
import hashlib
import json
import shutil
import zipfile

SOURCE_SUFFIXES = {'.h', '.hpp', '.cpp', '.inl', '.cs'}
LIBRARIES = ('libDaScript.lib', 'libDaScript_runtime.lib', 'libUriParser.lib')

def safe_name(name):
    path = PurePosixPath(name)
    if not name or name != path.as_posix() or not path.parts or '\\' in name or ':' in name or path.is_absolute() or '..' in path.parts:
        raise ValueError(f'Unsafe distribution path: {name}')
    return path

def forbidden(name):
    path = safe_name(name)
    return bool(set(p.lower() for p in path.parts) & {'tests', 'logs', 'binaries', 'intermediate', 'saved', '.git', '__pycache__'}) or path.suffix.lower() in {'.log', '.pdb', '.obj', '.pyc', '.tmp'} or path.name in {'DasIncludeTestFixtures.txt', 'DasIncludeEditorTests.txt'}

def runtime_files(root):
    content = root / 'Plugins/DasForUE/Content/Das'
    manifest = content / 'RuntimeScripts.txt'
    result = [manifest]
    for line in manifest.read_text(encoding='utf-8-sig').splitlines():
        line = line.strip()
        if not line or line.startswith('#'):
            continue
        safe_name(line)
        path = content / line
        if not path.is_file() or not path.resolve().is_relative_to(content.resolve()):
            raise ValueError(f'Missing/escaping runtime file: {line}')
        result.append(path)
    library = list((content / 'daslib').rglob('*.das'))
    if not library:
        raise ValueError('Missing daScript standard library')
    result.extend(library)
    return result

def select_files(root):
    root = Path(root)
    selected = {}
    def add(path, name=None):
        name = name or path.relative_to(root).as_posix()
        if forbidden(name) or not path.is_file():
            raise ValueError(f'Invalid distribution input: {name}')
        selected[name] = path
    for path in (root / 'Plugins/DasForUE/Source').rglob('*'):
        if path.is_file() and path.suffix in SOURCE_SUFFIXES:
            add(path)
    for module in ['DasRuntime', 'DasEditor']:
        add(root / f'Plugins/DasForUE/Source/{module}/{module}.Build.cs')
    add(root / 'Plugins/DasForUE/DasForUE.uplugin')
    for path in runtime_files(root):
        add(path)
    for directory in ['ThirdParty/daScript/include', 'ThirdParty/daScript/3rdparty/fmt/include']:
        for path in (root / directory).rglob('*'):
            if path.is_file() and not forbidden(path.relative_to(root).as_posix()):
                add(path)
    for library in LIBRARIES:
        add(root / 'ThirdParty/daScript/lib/Release' / library)
    for name in ['ThirdParty/daScript/LICENSE', 'ThirdParty/daScript/THIRD_PARTY_NOTICES.md',
                 'ThirdParty/daScript/3rdparty/fmt/LICENSE', 'ThirdParty/daScript/3rdparty/uriparser/COPYING',
                 'ThirdParty/daScript/3rdparty/uriparser/AUTHORS', 'LICENSE', 'dependencies.json']:
        add(root / name)
    add(root / 'Distribution/README.md', 'README.md')
    example = root / 'Examples/CleanGameplay'
    for relative in ['CleanGameplay.uproject', 'README.md', 'Config/DefaultEngine.ini', 'Config/DefaultGame.ini']:
        add(example / relative)
    for path in (example / 'Source').rglob('*'):
        if path.is_file() and path.suffix in SOURCE_SUFFIXES:
            add(path)
    for path in (example / 'Content/Das/Scripts').glob('*.das'):
        add(path)
    return dict(sorted(selected.items()))

def sha256(path):
    with path.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()

def build(root, output):
    selected = select_files(root)
    manifest = {'kind': 'DasForUE-source', 'files': {name: sha256(path) for name, path in selected.items()}}
    output = Path(output)
    output.parent.mkdir(parents=True, exist_ok=True)
    temporary = output.with_suffix('.zip.tmp')
    with zipfile.ZipFile(temporary, 'w', compression=zipfile.ZIP_DEFLATED, compresslevel=1) as archive:
        for name, path in selected.items():
            archive.write(path, name)
        archive.writestr('distribution-manifest.json', json.dumps(manifest, indent=2)+'\n')
    temporary.replace(output)
    return audit(output)

def audit(output):
    with zipfile.ZipFile(output) as archive:
        names = archive.namelist()
        if len(names) != len(set(names)):
            raise ValueError('Duplicate archive members')
        for name in names:
            if forbidden(name):
                raise ValueError(f'Forbidden distribution artifact: {name}')
        manifest = json.loads(archive.read('distribution-manifest.json'))
        if manifest.get('kind') != 'DasForUE-source' or set(names) != set(manifest['files']) | {'distribution-manifest.json'}:
            raise ValueError('Distribution manifest does not match archive')
        for name, expected in manifest['files'].items():
            with archive.open(name) as stream:
                actual = hashlib.file_digest(stream, 'sha256').hexdigest()
            if actual != expected:
                raise ValueError(f'Distribution hash mismatch: {name}')
        runtime = archive.read('Plugins/DasForUE/Content/Das/RuntimeScripts.txt').decode('utf-8-sig')
        approved = {line.strip() for line in runtime.splitlines() if line.strip() and not line.lstrip().startswith('#')}
        prefix = 'Plugins/DasForUE/Content/Das/'
        for name in names:
            if name.startswith(prefix):
                relative = name[len(prefix):]
                if relative != 'RuntimeScripts.txt' and relative not in approved and not (relative.startswith('daslib/') and relative.endswith('.das')):
                    raise ValueError(f'Unlisted plugin content: {name}')
        return manifest

def extract(output, destination):
    manifest = audit(output)
    destination = Path(destination).resolve()
    prior = destination / 'distribution-manifest.json'
    if destination.exists() and any(destination.iterdir()) and not prior.is_file():
        raise ValueError('Refusing to reuse a directory without a distribution manifest')
    destination.mkdir(parents=True, exist_ok=True)
    def target(name):
        path = destination.joinpath(*safe_name(name).parts)
        if not path.resolve().is_relative_to(destination):
            raise ValueError(f'Escaping extraction target: {name}')
        return path
    if prior.is_file():
        old = json.loads(prior.read_text())
        if old.get('kind') != 'DasForUE-source':
            raise ValueError('Unknown previous distribution')
        for name in set(old['files']) - set(manifest['files']):
            target(name).unlink(missing_ok=True)
    with zipfile.ZipFile(output) as archive:
        for name in archive.namelist():
            path = target(name)
            path.parent.mkdir(parents=True, exist_ok=True)
            if name in manifest['files'] and path.is_file() and sha256(path) == manifest['files'][name]:
                continue
            with archive.open(name) as source, path.open('wb') as out:
                shutil.copyfileobj(source, out)

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--root', type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument('--output', required=True, type=Path)
    parser.add_argument('--extract', type=Path)
    parser.add_argument('--audit-only', action='store_true')
    args = parser.parse_args()
    manifest = audit(args.output) if args.audit_only else build(args.root, args.output)
    if args.extract:
        extract(args.output, args.extract)
    print(f"DAS_SOURCE_DISTRIBUTION=PASS files={len(manifest['files'])} archive={args.output}")
