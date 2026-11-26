#!/bin/bash
# 用 pcb-rnd 实测我们导出的 gEDA 文件(在 WSL Ubuntu-24.04 里跑,需 apt install pcb-rnd librsvg2-bin):
#   加载布局 -> 导入网表 -> 为布线器生成飞线 -> 自动布线 -> 导出 Gerber/图片
# 通过标准:
#   · run.log 无 SHORT(短路)警告
#   · run.log 出现 "N of N nets successfully routed"(所有网络布通)
#   · fa_routed.pcb 里有 Line[ 走线段(自动布线真的动了手)
#   · 导出成套 Gerber(.gbr)
# 用法(在 WSL 里,路径为 Linux 路径):
#   bash /mnt/d/EDA工业软件/EDA-repo/tools/verify_geda_pcb-rnd.sh \
#        /mnt/d/EDA工业软件/EDA-repo/examples/subcircuits/gEDA导出/full_adder.net \
#        /mnt/d/EDA工业软件/EDA-repo/examples/subcircuits/gEDA导出/full_adder.pcb
set -e
SRC_NET="$1"
SRC_PCB="$2"
rm -rf /root/verify/geda_out && mkdir -p /root/verify/geda_out && cd /root/verify/geda_out
cp "$SRC_NET" fa.net
cp "$SRC_PCB" fa.pcb
printf "LoadFrom(Netlist, fa.net)\nAddRats(AllRats, manhattan)\nAutoRoute(AllRats)\nSaveTo(LayoutAs, fa_routed.pcb)\nExport(gerber)\nExport(svg, --outfile, fa_routed.svg)\nQuit()\n" | pcb-rnd --gui batch fa.pcb > run.log 2>&1 || true

echo "== run.log 关键行 =="
echo "SHORT 警告数: $(grep -c SHORT run.log)"
grep -iE "successfully routed|Total added wire length" run.log

python3 - <<'PY'
import re, os
s = open("fa_routed.pcb", encoding="utf-8", errors="replace").read()
print("布线段数(Line[):", len(re.findall(r"^\s*Line\[", s, re.M)), " 过孔数:", len(re.findall(r"^\s*Via\[", s, re.M)))
print("Gerber 文件数:", len([f for f in os.listdir(".") if f.endswith(".gbr")]))
PY

# SVG -> PNG(高分辨率,给文档/答辩用)
if command -v rsvg-convert >/dev/null && [ -f fa_routed.svg ]; then
  rsvg-convert -w 1600 fa_routed.svg -o fa_routed.png
  echo "渲染 PNG: $(stat -c%s fa_routed.png) bytes"
fi
echo "输出目录: /root/verify/geda_out  (fa_routed.pcb / fa_routed.svg / fa_routed.png / *.gbr / run.log)"
