import json
import re
import os


F = input()


settings = json.load(open("/home/a1024/cpp/contest/contest_true/library_expand/library_expand_info.json"))


registered_library = set(settings["registered_library"])
library_path = settings["library_path"]
source_file_path = settings["source_file_path"]
result_path = settings["result_path"]


used = set()

def dfs_expand(sourcecode):
    length_offset = 0
    includepos = [match.span() for match in re.finditer(r"# *include *\<.*\>", sourcecode)]
    res = [c for c in sourcecode]
    for p in includepos:
        temp = sourcecode[p[0]:p[1]]
        temp2 = temp[(temp.find("<")+1):temp.rfind(">")]
        if temp2 in used:
            res[(p[0]+length_offset):(p[1]+length_offset)] = []
            length_offset -= len(temp)
        elif temp2 in registered_library:
            used.add(temp2)
            for path in library_path:
                if os.path.isfile(f"{path}/{temp2}"):
                    print(f"expanded \"{temp}\"")
                    res2 = dfs_expand(open(f"{path}/{temp2}").read())
                    res[(p[0]+length_offset):(p[1]+length_offset)] = res2
                    length_offset += len(res2)-len(temp)
                    break
            else:
                raise ValueError("指定されたライブラリは指定されたパス内に存在しません")
    return res

with open(f"{result_path}/{F[:F.find(".")]}_submit.cpp", "w") as result:
    result.write("".join(dfs_expand(open(f"{source_file_path}/{F}").read())))

print("展開が正常に終了しました")