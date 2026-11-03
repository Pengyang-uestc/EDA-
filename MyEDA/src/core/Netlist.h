#pragma once
#include "core/CircuitValidation.h"
#include <map>
#include <vector>
#include <set>
using NetPin=std::pair<int,int>;
inline std::vector<std::vector<std::pair<int,int>>> BuildNets(const wxVector<Wire>& wires)
{
    typedef std::pair<int,int> Pin;      // (元件编号, 引脚号)
    std::map<Pin, Pin> parent;           // 并查集:引脚 → 它的"上级"

    // 第 1 步:每个引脚先自成一派
    for (size_t i = 0; i < wires.size(); i++) {
        Pin a(wires[i].comp1, wires[i].pin1);
        Pin b(wires[i].comp2, wires[i].pin2);
        parent[a] = a;
        parent[b] = b;
    }
    // 找"族长"(顺手做路径压缩,让下次查找更快)
    auto findRoot = [&parent](Pin p) {
        Pin root = p;
        while (parent[root] != root) root = parent[root];
        while (parent[p] != root) { Pin next = parent[p]; parent[p] = root; p = next; }
        return root;
    };
    // 第 2 步:每根连线把两端并成一家
    for (size_t i = 0; i < wires.size(); i++) {
        Pin ra = findRoot(Pin(wires[i].comp1, wires[i].pin1));
        Pin rb = findRoot(Pin(wires[i].comp2, wires[i].pin2));
        if (ra != rb) parent[ra] = rb;
    }
    // 第 3 步:同一个族长的引脚归为一个网络
    std::map<Pin, std::vector<Pin>> groups;
    for (std::map<Pin, Pin>::iterator it = parent.begin(); it != parent.end(); ++it)
        groups[findRoot(it->first)].push_back(it->first);

    std::vector<std::vector<Pin>> nets;
    for (std::map<Pin, std::vector<Pin>>::iterator it = groups.begin(); it != groups.end(); ++it) {
        std::vector<Pin> nodes = it->second;
        std::sort(nodes.begin(), nodes.end());   // 排序 → 输出稳定,便于往返比对
        nets.push_back(nodes);
    }
    std::sort(nets.begin(), nets.end());
    return nets;
}


struct ElectricalNet { int code; std::vector<NetPin> pins,drivers,receivers; };
struct CircuitNetlist {
 std::vector<ElectricalNet> nets;
 std::vector<NetPin> unconnected;
 std::vector<wxString> errors,warnings;
 bool valid() const {return errors.empty();}
};
inline CircuitNetlist BuildCircuitNetlist(const wxVector<Component>& components,const wxVector<Wire>& wires) {
 CircuitNetlist result;std::map<int,const Component*> ids;std::set<NetPin> connected;
 for(const auto& c:components)if(c.id<=0 || !ids.emplace(c.id,&c).second)result.errors.push_back("元件编号重复或无效");
 for(const auto& w:wires)for(const auto& p:std::vector<NetPin>{{w.comp1,w.pin1},{w.comp2,w.pin2}}) {
  auto it=ids.find(p.first);
  if(it==ids.end() || p.second<0 || p.second>=GatePinCount(*it->second))result.errors.push_back("连线引用了不存在的元件或引脚");
  else connected.insert(p);
 }
 if(!result.valid())return result;
 int code=1;
 for(const auto& pins:BuildNets(wires)) {
  ElectricalNet net;net.code=code++;net.pins=pins;
  for(const auto& pin:pins)(IsOutputPin(*ids[pin.first],pin.second)?net.drivers:net.receivers).push_back(pin);
  if(net.drivers.size()>1)result.errors.push_back(wxString::Format("网络 %d 存在多个驱动端",net.code));
  if(net.drivers.empty())result.warnings.push_back(wxString::Format("网络 %d 没有驱动端",net.code));
  result.nets.push_back(net);
 }
 for(const auto& c:components)for(int pin=0;pin<GatePinCount(c);++pin)if(!connected.count({c.id,pin}))result.unconnected.push_back({c.id,pin});
 return result;
}
