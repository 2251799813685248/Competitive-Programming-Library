import json
import re
import os
import subprocess


def copy_to_windows_clipboard(text):
    process = subprocess.Popen(['clip.exe'], stdin=subprocess.PIPE)
    encoded_text = text.encode('cp932', errors='replace')
    process.communicate(input=encoded_text)
    print("提出用ソースコードをクリップボードにコピーしました")


F = input()


settings = json.load(open("/home/a1024/cpp/contest/library_expand/library_expand_info.json"))


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

expanded_sourcecode = "".join(dfs_expand(open(f"{source_file_path}/{F}").read()))
with open(f"{result_path}/{F[:F.find(".")]}_submit.cpp", "w") as result:
    result.write(expanded_sourcecode)

print(f"展開先: {F[:F.find(".")]}_submit.cpp")
copy_to_windows_clipboard(expanded_sourcecode)