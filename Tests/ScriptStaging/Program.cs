using System;
using System.IO;
using System.Linq;
using System.Text.RegularExpressions;
class Program
{
    static void Check(bool ok, string message) { if (!ok) throw new Exception(message); }
    static void Refuses(Action action) { bool refused=false; try { action(); } catch (Exception e) when (e is IOException || e is System.IO.InvalidDataException) { refused=true; } Check(refused,"Expected staging refusal"); }
    static void Main(string[] args)
    {
        string repo=args[0], root=Path.GetFullPath(Path.Combine(repo,"Plugins/DasForUE/Content/Das"));
        string[] production=DasScriptStaging.SelectPluginFiles(root,null);
        Check(production.Length>100,"Runtime library missing");
        Check(!production.Any(p=>Path.GetFileName(p).StartsWith("dsl_")),"DSL fixture leaked");
        Check(!production.Any(p=>Path.GetFileName(p)=="ue_aot_emit.das"),"Build-time emitter leaked");
        Check(production.All(p=>Path.GetExtension(p)==".das"),"Non-script artifact leaked");
        var selected=production.ToHashSet(StringComparer.OrdinalIgnoreCase);
        foreach (string file in production.Where(p=>!p.Contains(Path.DirectorySeparatorChar+"daslib"+Path.DirectorySeparatorChar)))
            foreach (Match dep in Regex.Matches(File.ReadAllText(file),@"(?m)^\s*require\s+([\w/]+)"))
            {
                string local=Path.GetFullPath(Path.Combine(root,"Scripts",dep.Groups[1].Value+".das"));
                string runtime=Path.GetFullPath(Path.Combine(root,"Runtime",dep.Groups[1].Value+".das"));
                if(File.Exists(runtime)) local=runtime;
                if(File.Exists(local)) Check(selected.Contains(local),"Unstaged local require: "+local);
            }
        string[] smoke=DasScriptStaging.SelectPluginFiles(root,Path.Combine(repo,"Tests/DasForUESmoke/Config/DasPluginScripts.txt"));
        Check(smoke.Any(p=>Path.GetFileName(p)=="dsl_playable_character.das"),"Smoke fixtures lost");
        string temp=Path.Combine(Path.GetTempPath(),"das-staging-"+Guid.NewGuid().ToString("N"));
        Directory.CreateDirectory(Path.Combine(temp,"Scripts"));Directory.CreateDirectory(Path.Combine(temp,"daslib"));Directory.CreateDirectory(Path.Combine(temp,"Config"));
        try {
            File.WriteAllText(temp+".das","");
            File.WriteAllText(Path.Combine(temp,"RuntimeScripts.txt"),"Scripts/core.das\n");
            foreach(string name in new[]{"core.das","dsl_registered_random.das","backup.das.bak","catalog.json"}) File.WriteAllText(Path.Combine(temp,"Scripts",name),"");
            File.WriteAllText(Path.Combine(temp,"daslib","library.das"),"");
            File.WriteAllText(Path.Combine(temp,"daslib","notes.md"),"");
            string extras=Path.Combine(temp,"Config","DasPluginScripts.txt");
            Check(DasScriptStaging.SelectPluginFiles(temp,extras).Length==2,"Default selection depends on directory debris");
            File.WriteAllText(extras,"Scripts/catalog.json\nScripts/core.das\n");
            Check(DasScriptStaging.SelectPluginFiles(temp,extras).Length==3,"Explicit data/duplicate selection");
            foreach(string invalid in new[]{"../"+Path.GetFileName(temp)+".das",Path.GetFullPath(extras),"Scripts/*.das","Scripts/missing.das"}) {
                File.WriteAllText(extras,invalid);Refuses(()=>DasScriptStaging.SelectPluginFiles(temp,extras));
            }
            File.WriteAllText(extras,"");
            File.WriteAllText(Path.Combine(temp,"Config","DasIncludeTestFixtures.txt"),"include-plugin-test-fixtures");
            var test=DasScriptStaging.SelectPluginFiles(temp,extras);
            Check(test.Any(p=>p.EndsWith("dsl_registered_random.das")),"Explicit test opt-in missing");
            Check(!test.Any(p=>p.EndsWith(".bak")),"Backup staged even in test mode");
            File.WriteAllText(Path.Combine(temp,"Config","DasIncludeTestFixtures.txt"),"typo");Refuses(()=>DasScriptStaging.SelectPluginFiles(temp,extras));
            File.Delete(Path.Combine(temp,"RuntimeScripts.txt"));Refuses(()=>DasScriptStaging.SelectPluginFiles(temp,null));
        } finally { Directory.Delete(temp,true); File.Delete(temp+".das"); }
        Directory.CreateDirectory(Path.Combine(repo,"logs"));
        File.WriteAllLines(Path.Combine(repo,"logs/staging-smoke-files.txt"),smoke);
        File.WriteAllLines(Path.Combine(repo,"logs/staging-production-files.txt"),production);
        Console.WriteLine($"DAS_SCRIPT_STAGING=PASS production={production.Length} smoke={smoke.Length}");
    }
}
