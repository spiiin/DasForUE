import json,sys
p=sys.argv[1]
d=json.load(open(p,encoding='utf-8-sig'))
open(p,'w',encoding='utf-8',newline='\n').write(json.dumps(d,indent=2,ensure_ascii=False)+'\n')
