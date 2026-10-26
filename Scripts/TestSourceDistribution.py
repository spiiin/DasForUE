from pathlib import Path
import tempfile
import unittest
import zipfile
import BuildSourceDistribution as delivery

class SourceDistributionTest(unittest.TestCase):
    def setUp(self):
        self.temp=tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root=Path(self.temp.name)/"repo"
        required=["Plugins/DasForUE/DasForUE.uplugin", "Plugins/DasForUE/Source/DasRuntime/DasRuntime.Build.cs", "Plugins/DasForUE/Source/DasEditor/DasEditor.Build.cs", "Plugins/DasForUE/Content/Das/daslib/ast.das", "Plugins/DasForUE/Source/DasRuntime/Runtime.cpp",
            "Plugins/DasForUE/Content/Das/Scripts/ue_dsl.das", "ThirdParty/daScript/include/core.h",
            "ThirdParty/daScript/3rdparty/fmt/include/fmt/core.h", "ThirdParty/daScript/LICENSE",
            "ThirdParty/daScript/THIRD_PARTY_NOTICES.md", "ThirdParty/daScript/3rdparty/fmt/LICENSE",
            "ThirdParty/daScript/3rdparty/uriparser/COPYING", "ThirdParty/daScript/3rdparty/uriparser/AUTHORS",
            "LICENSE", "dependencies.json", "Distribution/README.md", "Examples/CleanGameplay/CleanGameplay.uproject",
            "Examples/CleanGameplay/README.md", "Examples/CleanGameplay/Config/DefaultEngine.ini", "Examples/CleanGameplay/Config/DefaultGame.ini"]
        required += ["ThirdParty/daScript/lib/Release/"+name for name in delivery.LIBRARIES]
        for name in required:self.write(name,"fixture")
        self.write("Plugins/DasForUE/Content/Das/RuntimeScripts.txt","Scripts/ue_dsl.das")
        self.output=Path(self.temp.name)/"delivery.zip"
    def write(self,name,text):
        path=self.root/name;path.parent.mkdir(parents=True,exist_ok=True);path.write_text(text)
    def test_selection_excludes_fixture_sources_scripts_and_build_artifacts(self):
        for name in ["Plugins/DasForUE/Tests/Editor/Test.cpp","Plugins/DasForUE/Tests/Runtime/Probe.h",
                     "Plugins/DasForUE/Content/Das/Scripts/dsl_fixture.das","Plugins/DasForUE/Binaries/test.dll",
                     "Plugins/DasForUE/logs/compiler.log","Examples/CleanGameplay/Config/DasIncludeEditorTests.txt"]:
            self.write(name,"must not ship")
        selected=delivery.select_files(self.root)
        self.assertFalse(any("Tests/" in name or "fixture" in name or name.endswith(".log") for name in selected))
        self.assertNotIn("Examples/CleanGameplay/Config/DasIncludeEditorTests.txt",selected)
        delivery.build(self.root,self.output)
        delivery.extract(self.output,Path(self.temp.name)/"verify")
    def test_auditor_rejects_extra_fixture_even_if_archive_is_readable(self):
        delivery.build(self.root,self.output)
        with zipfile.ZipFile(self.output,"a") as archive:archive.writestr("Plugins/DasForUE/Tests/Editor/Test.cpp","leak")
        with self.assertRaisesRegex(ValueError,"Forbidden"):delivery.audit(self.output)
    def test_auditor_rejects_unlisted_content(self):
        delivery.build(self.root,self.output)
        with zipfile.ZipFile(self.output,"a") as archive:archive.writestr("Plugins/DasForUE/Content/Das/Scripts/stray.das","leak")
        with self.assertRaisesRegex(ValueError,"manifest"):delivery.audit(self.output)
    def test_paths_cannot_escape_bundle(self):
        for path in ["../outside", "C:/outside", "/outside", "a//b", "a/./b", "a\\b"]:
            with self.assertRaises(ValueError):delivery.safe_name(path)
    def test_missing_standard_library_is_rejected(self):
        (self.root/"Plugins/DasForUE/Content/Das/daslib/ast.das").unlink()
        with self.assertRaisesRegex(ValueError,"standard library"):delivery.select_files(self.root)
    def test_hash_mismatch_is_rejected(self):
        delivery.build(self.root,self.output)
        with zipfile.ZipFile(self.output) as archive:
            files={name:archive.read(name) for name in archive.namelist()}
        files["README.md"]=b"corrupted"
        with zipfile.ZipFile(self.output,"w") as archive:
            for name,data in files.items():archive.writestr(name,data)
        with self.assertRaisesRegex(ValueError,"hash mismatch"):delivery.audit(self.output)
    def test_extraction_does_not_overwrite_unrelated_directory(self):
        delivery.build(self.root,self.output)
        target=Path(self.temp.name)/"other";target.mkdir();(target/"user.txt").write_text("keep")
        with self.assertRaisesRegex(ValueError,"Refusing"):delivery.extract(self.output,target)
        self.assertEqual((target/"user.txt").read_text(),"keep")

if __name__=="__main__":unittest.main()
