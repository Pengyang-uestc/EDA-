"""半加器 KiCad 板脚本走线(Lee 网格路由器)。
从板子读真实焊盘位置与真实网络号,逐网络把同网络焊盘用折线连成树,
自动绕开所有其他焊盘(含空网络焊盘)与其他网络的走线,走线归属正确网络号。
输出 routed_half_adder.kicad_pcb,随后用 kicad-cli pcb drc 复核。
"""
import pcbnew
from collections import deque

SRC = "/root/kicad_verify/half_adder.kicad_pcb"
OUT = "/root/kicad_verify/routed_half_adder.kicad_pcb"
TRACK_MM = 0.5
CLEAR_MM = 0.25
STEP = 0.25

board = pcbnew.LoadBoard(SRC)

# ---------- 焊盘 ----------
pad_info = []   # (x,y,r,netcode)
for fp in board.GetFootprints():
    for p in fp.Pads():
        pos = p.GetPosition(); sz = p.GetSize()
        pad_info.append((pcbnew.ToMM(pos.x), pcbnew.ToMM(pos.y),
                         pcbnew.ToMM(max(sz.x, sz.y)) / 2.0, p.GetNetCode()))

xs = [p[0] for p in pad_info]; ys = [p[1] for p in pad_info]
X0, Y0 = min(xs) - 3.0, min(ys) - 3.0
NX = int((max(xs) - min(xs) + 6.0) / STEP) + 1
NY = int((max(ys) - min(ys) + 6.0) / STEP) + 1
def ci(x): return int(round((x - X0) / STEP))
def cy(y): return int(round((y - Y0) / STEP))

# cellpadnets[cell] = 该格被哪些网络的焊盘 keepout 覆盖(含空网络=0)
cellpadnets = {}
pads_by_net = {}
for (x, y, r, nc) in pad_info:
    if nc > 0:
        pads_by_net.setdefault(nc, []).append((ci(x), cy(y)))
    kc = int(round((r + TRACK_MM/2 + CLEAR_MM + 0.1) / STEP))
    pcx, pcy = ci(x), cy(y)
    for dx in range(-kc, kc+1):
        for dy in range(-kc, kc+1):
            if dx*dx + dy*dy <= kc*kc:
                gx, gy = pcx+dx, pcy+dy
                if 0 <= gx < NX and 0 <= gy < NY:
                    cellpadnets.setdefault((gx, gy), set()).add(nc)

# 走线占用:trackown[layer][cell] = 网络号;halo 区记 -net(仅阻挡其他网络)。分层互不影响。
trackown = {"F.Cu": {}, "B.Cu": {}}

def blocked(net, layer, c):
    # 焊盘阻挡:本格被任一"非本网络"焊盘 keepout 覆盖(焊盘是通孔,两层都算)
    for m in cellpadnets.get(c, ()):
        if m != net:
            return True
    own = trackown[layer]
    o = own.get(c)
    if o is not None and o != net and o != -net:
        return True
    return False

def lee(sources, goals, net, layer):
    prev = {c: None for c in sources}
    q = deque(sources)
    while q:
        c = q.popleft()
        if c in goals and prev.get(c) is not None:
            path = []; cur = c
            while cur is not None:
                path.append(cur); cur = prev[cur]
            return list(reversed(path))
        for nb in neighbors(c):
            if nb in prev: continue
            if blocked(net, layer, nb): continue
            prev[nb] = c; q.append(nb)
    return None

def neighbors(c):
    x, y = c
    for dx, dy in ((1,0),(-1,0),(0,1),(0,-1)):
        nx, ny = x+dx, y+dy
        if 0 <= nx < NX and 0 <= ny < NY:
            yield nx, ny

def mark_path(path, net, layer):
    halo = int(round((TRACK_MM/2 + CLEAR_MM) / STEP))
    own = trackown[layer]
    for (x, y) in path:
        own[(x, y)] = net
        for dx in range(-halo, halo+1):
            for dy in range(-halo, halo+1):
                gx, gy = x+dx, y+dy
                if (gx, gy) not in own:
                    own[(gx, gy)] = -net

def path_to_segments(path, net, layer):
    if len(path) < 2: return
    segs = []
    start = path[0]
    for i in range(2, len(path)):
        d1 = (path[i-1][0]-path[i-2][0], path[i-1][1]-path[i-2][1])
        d2 = (path[i][0]-path[i-1][0], path[i][1]-path[i-1][1])
        if d1 != d2:
            segs.append((start, path[i-1])); start = path[i-1]
    segs.append((start, path[-1]))
    for a, b in segs:
        segments.append((net, layer, X0+a[0]*STEP, Y0+a[1]*STEP, X0+b[0]*STEP, Y0+b[1]*STEP))

segments = []   # (net, layer, x1,y1,x2,y2)
print("网络:", sorted(pads_by_net.keys()))
for net in sorted(pads_by_net.keys()):
    padset = pads_by_net[net]
    placed = None
    for layer in ("F.Cu", "B.Cu"):
        connected = [padset[0]]
        remaining = list(padset[1:])
        segs0 = len(segments)
        ok = True
        while remaining:
            path = lee(list(connected), set(remaining), net, layer)
            if path is None:
                ok = False; break
            mark_path(path, net, layer)
            path_to_segments(path, net, layer)
            end = path[-1]
            remaining.remove(end); connected.append(end)
        if ok:
            placed = layer
            print(f"网络 {net}: {len(padset)} 焊盘 -> {layer} 已连通,本网络走线段 {len(segments)-segs0}")
            break
        else:
            # 回滚本层已标记的走线,换层重试
            segments = segments[:segs0]
            trackown[layer] = {}
            print(f"网络 {net}: {layer} 布不通,改试另一层")
    if placed is None:
        print(f"!! 网络 {net} 两层都布不通")

# ---------- 写回 KiCad ----------
for t in list(board.GetTracks()):
    board.Delete(t)
w = pcbnew.FromMM(TRACK_MM)
for (net, layername, x1, y1, x2, y2) in segments:
    t = pcbnew.PCB_TRACK(board)
    t.SetStart(pcbnew.VECTOR2I(pcbnew.FromMM(x1), pcbnew.FromMM(y1)))
    t.SetEnd(pcbnew.VECTOR2I(pcbnew.FromMM(x2), pcbnew.FromMM(y2)))
    t.SetWidth(w); t.SetLayer(board.GetLayerID(layername)); t.SetNetCode(net)
    board.Add(t)
# 补板框(原板没有 Edge.Cuts,DRC 会报 invalid_outline):4 条边线
mx = pcbnew.FromMM(min(xs) - 4.0); my = pcbnew.FromMM(min(ys) - 4.0)
Mx = pcbnew.FromMM(max(xs) + 4.0); My = pcbnew.FromMM(max(ys) + 4.0)
edge_layer = board.GetLayerID("Edge.Cuts")
for (ax, ay, bx, by) in ((mx,my,Mx,my),(Mx,my,Mx,My),(Mx,My,mx,My),(mx,My,mx,my)):
    e = pcbnew.PCB_SHAPE(board)
    e.SetShape(pcbnew.SHAPE_T_SEGMENT)
    e.SetStart(pcbnew.VECTOR2I(ax, ay)); e.SetEnd(pcbnew.VECTOR2I(bx, by))
    e.SetLayer(edge_layer); e.SetWidth(pcbnew.FromMM(0.1))
    board.Add(e)

board.Save(OUT)
print(f"写出 {OUT},走线段 {len(segments)} 条")