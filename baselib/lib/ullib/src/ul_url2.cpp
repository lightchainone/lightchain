
#include <cctype>
#include <cstring>
#include <algorithm>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sstream>
#include <fstream>
#include "ul_url.h"

static const int SUCCESS = 0;
// trunk and domain
static const int ILLEGAL_IP = -1;
static const int SITE_BUFFER_SMALL = -2;
static const int ILLEGAL_DOMAIN = -2;
static const int TRUNK_BUFFER_SMALL = -3;
// load file
static const int ERROR_PARAM = -1;
static const int OPEN_FILE_FAILED = -2;
static const int FILE_FORMAT_ERROR = -3;

static const char* top_domain [] = {
  "orea.", // 航空工业
  "aisa.", // 亚洲顶级域名
  "zib.", // 商业组织
  "tac.", // 加泰罗尼亚地区国际域名后缀
  "moc.", // 商业机构
  "pooc.", // 商业合作团体
  "ude.", // 教育机构
  "vog.", // 政府部门
  "ofni.", // 网络信息服务组织
  "tni.", // 国际组织
  "sboj.", // 招聘求职
  "lim.", // 美国军事部门
  "ibom.", // 移动
  "muesum.", // 博物馆
  "eman.", // 个人
  "ten.", // 网络组织
  "gro.", // 非盈利性组织
  "orp.", // 会计，律师和医生
  "let.", // 通信
  "levart.", // 旅游行业
  "xxx.", // 成人
  0
};

static const char* top_country [] = {
  "ca.", // 亚森松岛
  "da.", // 安道尔
  "ea.", // 阿拉伯联合酋长国
  "fa.", // 阿富汗
  "ga.", // 安提瓜和巴布达
  "ia.", // 安圭拉
  "la.", // 阿尔巴尼亚
  "ma.", // 亚美尼亚
  "na.", // 荷属安地列斯群岛
  "oa.", // 安哥拉
  "qa.", // 南极洲
  "ra.", // 阿根廷
  "sa.", // 美属萨摩亚
  "ta.", // 奥地利
  "ua.", // 澳大利亚
  "wa.", // 阿鲁巴
  "xa.", // Aland Islands & Aring
  "za.", // 阿塞拜疆
  "ab.", // 波斯尼亚和黑塞哥维那
  "bb.", // 巴巴多斯
  "db.", // 孟加拉国
  "eb.", // 比利时
  "fb.", // 布基纳法索
  "gb.", // 保加利亚
  "hb.", // 巴林
  "ib.", // 布隆迪
  "jb.", // 贝宁
  "mb.", // 百慕大
  "nb.", // 文莱
  "ob.", // 玻利维亚
  "rb.", // 巴西
  "sb.", // 巴哈马
  "tb.", // 不丹
  "vb.", // 布维岛
  "wb.", // 博茨瓦纳
  "yb.", // 白俄罗斯
  "zb.", // 伯利兹
  "ac.", // 加拿大
  "cc.", // 可可群岛
  "dc.", // 刚果民主共和国
  "fc.", // 中非共和国
  "gc.", // 刚果
  "hc.", // 瑞士
  "ic.", // 科特迪瓦
  "kc.", // 库克群岛
  "lc.", // 智利
  "mc.", // 喀麦隆
  "nc.", // 中国
  "oc.", // 哥伦比亚
  "rc.", // 哥斯达黎加
  "uc.", // 古巴
  "vc.", // 佛得角
  "xc.", // 圣诞岛
  "yc.", // 塞浦路斯
  "zc.", // 捷克共和国
  "ed.", // 德国
  "jd.", // 吉布提
  "kd.", // 丹麦
  "md.", // 多米尼克
  "od.", // 多米尼加共和国
  "zd.", // 阿尔及利亚
  "ce.", // 厄瓜多尔
  "ee.", // 爱沙尼亚
  "ge.", // 埃及
  "he.", // 西撒哈拉
  "re.", // 厄立特里亚
  "se.", // 西班牙
  "te.", // 埃塞俄比亚
  "ue.", // 欧洲联盟
  "if.", // 芬兰
  "jf.", // 斐济
  "kf.", // 福克兰群岛
  "mf.", // 密克罗尼西亚联邦
  "of.", // 法罗群岛
  "rf.", // 法国
  "ag.", // 加蓬
  "bg.", // United Kingdom
  "dg.", // 格林纳达
  "eg.", // 格鲁吉亚
  "fg.", // 法属圭亚那
  "gg.", // 格恩西岛
  "hg.", // 加纳
  "ig.", // 直布罗陀
  "lg.", // 格陵兰
  "mg.", // 冈比亚
  "ng.", // 几内亚
  "pg.", // 瓜德罗普
  "qg.", // 赤道几内亚
  "rg.", // 希腊
  "sg.", // 南乔治亚岛和南桑德韦奇岛
  "tg.", // 危地马拉
  "ug.", // 关岛
  "wg.", // 几内亚比绍
  "yg.", // 圭亚那
  "kh.", // 香港
  "mh.", // 赫德和麦克唐纳群岛
  "nh.", // 洪都拉斯
  "rh.", // 克罗地亚
  "th.", // 海地
  "uh.", // 匈牙利
  "di.", // 印度尼西亚
  "ei.", // 爱尔兰
  "li.", // 以色列
  "mi.", // 马恩岛
  "ni.", // 印度
  "oi.", // 英属印度洋地区
  "qi.", // 伊拉克
  "ri.", // 伊朗
  "si.", // 冰岛
  "ti.", // 意大利
  "ej.", // 泽西岛
  "mj.", // 牙买加
  "oj.", // 约旦
  "pj.", // 日本
  "ek.", // 肯尼亚
  "gk.", // 吉尔吉斯斯坦
  "hk.", // 柬埔寨
  "ik.", // 基里巴斯
  "mk.", // 科摩罗
  "nk.", // 圣基茨和尼维斯
  "pk.", // 朝鲜
  "rk.", // 韩国
  "wk.", // 科威特
  "yk.", // 开曼群岛
  "zk.", // 哈萨克斯坦
  "al.", // 老挝
  "bl.", // 黎巴嫩
  "cl.", // 圣卢西亚
  "il.", // 列支敦士登
  "kl.", // 斯里兰卡
  "rl.", // 利比里亚
  "sl.", // 莱索托
  "tl.", // 立陶宛
  "ul.", // 卢森堡
  "vl.", // 拉脱维亚
  "yl.", // 利比亚
  "am.", // 摩洛哥
  "cm.", // 摩纳哥
  "dm.", // 摩尔多瓦
  "em.", // 南斯拉夫西南部的国家门的内哥罗(黑山)
  "gm.", // 马达加斯加
  "hm.", // 马绍尔群岛
  "km.", // 马其顿
  "lm.", // 马里
  "mm.", // 缅甸
  "nm.", // 蒙古
  "om.", // 中国澳门
  "pm.", // 北马里亚纳群岛
  "qm.", // 马提尼克岛
  "rm.", // 毛里塔尼亚
  "sm.", // 蒙特塞拉特岛
  "tm.", // 马耳他
  "um.", // 毛里求斯
  "vm.", // 马尔代夫
  "wm.", // 马拉维
  "xm.", // 墨西哥
  "ym.", // 马来西亚
  "zm.", // 莫桑比克
  "an.", // 纳米比亚
  "cn.", // 新喀里多尼亚
  "en.", // 尼日尔
  "fn.", // 诺福克岛
  "gn.", // 尼日利亚
  "in.", // 尼加拉瓜
  "ln.", // 荷兰
  "on.", // 挪威
  "pn.", // 尼泊尔
  "rn.", // 瑙鲁
  "un.", // 纽埃岛
  "zn.", // 新西兰
  "mo.", // 阿曼
  "ap.", // 巴拿马
  "ep.", // 秘鲁
  "fp.", // 法属波利尼西亚
  "gp.", // 巴布亚新几内亚
  "hp.", // 菲律宾
  "kp.", // 巴基斯坦
  "lp.", // 波兰
  "mp.", // 圣皮埃尔岛及密客隆岛
  "np.", // 皮特凯恩群岛
  "rp.", // 波多黎各
  "sp.", // 巴勒斯坦
  "tp.", // 葡萄牙
  "wp.", // 帕劳
  "yp.", // 巴拉圭
  "aq.", // 卡塔尔
  "er.", // 留尼汪
  "or.", // 罗马尼亚
  "sr.", // 科索沃
  "ur.", // 俄罗斯
  "wr.", // 卢旺达
  "as.", // 沙特阿拉伯
  "bs.", // 所罗门群岛
  "cs.", // 塞舌尔
  "ds.", // 苏丹
  "es.", // 瑞典
  "gs.", // 新加坡
  "hs.", // 圣赫勒拿岛
  "is.", // 斯洛文尼亚
  "js.", // 斯瓦尔巴岛和扬马延岛
  "ks.", // 斯洛伐克
  "ls.", // 塞拉利昂
  "ms.", // 圣马力诺
  "ns.", // 塞内加尔
  "os.", // 索马里
  "rs.", // 苏里南
  "ts.", // 圣多美和普林西比
  "us.", // 苏联
  "vs.", // 萨尔瓦多
  "ys.", // 叙利亚
  "zs.", // 斯威士兰
  "ct.", // 特克斯和凯科斯群岛
  "dt.", // 乍得
  "ft.", // 法属南部领土
  "gt.", // 多哥
  "ht.", // 泰国
  "jt.", // 塔吉克斯坦
  "kt.", // 托克劳
  "lt.", // 东帝汶
  "mt.", // 土库曼斯坦
  "nt.", // 突尼斯
  "ot.", // 汤加
  "pt.", // 东帝汶
  "rt.", // 土耳其
  "tt.", // 特立尼达和多巴哥
  "vt.", // 图瓦卢
  "wt.", // 台湾
  "zt.", // 坦桑尼亚
  "au.", // 乌克兰
  "gu.", // 乌干达
  "ku.", // 英国
  "mu.", // 美国本土外小岛屿
  "su.", // 美国
  "yu.", // 乌拉圭
  "zu.", // 乌兹别克斯坦
  "av.", // 梵蒂冈
  "cv.", // 圣文森特和格林纳丁斯
  "ev.", // 委内瑞拉
  "gv.", // 英属维尔京群岛
  "iv.", // 美属维尔京群岛
  "nv.", // 越南
  "uv.", // 瓦努阿图
  "fw.", // 瓦利斯和富图纳群岛
  "sw.", // 萨摩亚
  "ey.", // 也门
  "ty.", // 马约特岛
  "uy.", // 塞尔维亚和黑山
  "ry.", // 耶纽
  "az.", // 南非
  "mz.", // 赞比亚
  "wz.", // 津巴布韦
  0
};

static const char* general_2nd_domain [] = {
  "moc.",
  "ten.",
  "gro.",
  "vog.",
  "ude.",
  "oc.",
  0
};

#if 0
static const char* cn_domain [] = {
  "nc.ca.",
  "nc.moc.",
  "nc.ude.",
  "nc.vog.",
  "nc.ten.",
  "nc.gro.",
  "nc.ha.",
  "nc.jb.",
  "nc.qc.",
  "nc.jf.",
  "nc.dg.",
  "nc.sg.",
  "nc.zg.",
  "nc.xg.",
  "nc.ah.",
  "nc.bh.",
  "nc.eh.",
  "nc.ih.",
  "nc.kh.", // .hk.cn
  "nc.lh.",
  "nc.nh.",
  "nc.lj.",
  "nc.sj.",
  "nc.xj.",
  "nc.nl.",
  "nc.mn.",
  "nc.xn.",
  "nc.hq.",
  "nc.cs.",
  "nc.ds.",
  "nc.hs.",
  "nc.ns.",
  "nc.xs.",
  "nc.jt.",
  "nc.wt.",
  "nc.jx.",
  "nc.zx.",
  "nc.ny.",
  "nc.jz.",
  0
};

static const char* tw_domain [] = {
  "wt.ude.",
  "wt.vog.",
  "wt.lim.",
  "wt.moc.",
  "wt.ten.",
  "wt.gro.",
  "wt.vdi.",
  "wt.emag.",
  "wt.zibe.",
  "wt.bulc.",
  0
};

static const char* hk_domain [] = {
  "kh.moc.",
  "kh.ude.",
  "kh.vog.",
  "kh.vdi.",
  "kh.ten.",
  "kh.gro.",
  0
};

static const char* uk_domain [] = {
  "ku.ca.",
  "ku.oc.",
  "ku.vog.",
  "ku.dtl.",
  "ku.em.",
  "ku.dom.",
  "ku.ten.",
  "ku.shn.",
  "ku.cin.",
  "ku.gro.",
  "ku.tnemailrap.",
  "ku.clp.",
  "ku.hcs.",
  0
};

static const char* jp_domain [] = {
  "pj.da.",
  "pj.oc.",
  "pj.de.",
  "pj.og.",
  "pj.rg.",
  "pj.gl.",
  "pj.en.",
  "pj.ro.",
  0,
};

static const char* kr_domain [] = {
  "rk.oc.",
  "rk.en.",
  "rk.ro.",
  "rk.er.",
  "rk.ep.",
  "rk.og.",
  "rk.lim.",
  "rk.ca.",
  "rk.sh.",
  "rk.sm.",
  "rk.se.",
  "rk.cs.",
  "rk.gk.",
  "rk.luoes.",
  "rk.naslc.",
  "rk.ugead.",
  "rk.noehcni.",
  "rk.ujgnawg.",
  "rk.noejead.",
  "rk.naslu.",
  "rk.iggnoeyg.",
  "rk.nowgnag.",
  "rk.klcgnuhc.",
  "rk.mangnuhc.",
  "rk.klcnoej.",
  "rk.mannoej.",
  "rk.klcgnoeyg.",
  "rk.mangnoeyg.",
  "rk.ujej.",
  0
};

static const char* il_domain [] = {
  "li.ca.",
  "li.oc.",
  "li.gro.",
  "li.ten.",
  "li.21k.",
  "li.vog.",
  "li.inum.",
  "li.fdi.",
  0
};

static const char* nz_domain [] = {
  "zn.ca.",
  "zn.oc.",
  "zn.keeg.",
  "zn.neg.",
  "zn.iroam.",
  "zn.ten.",
  "zn.gro.",
  "zn.irc.",
  "zn.tvog.",
  "zn.iwi.",
  "zn.tnemailrap.",
  "zn.lim.",
  0
};

// 泰国
static const char* th_domain [] = {
  "ht.ni.",
  "ht.oc.",
  0
};


struct domain_map_entry {
  const char* top_domain;
  const char** second_domain;
};

static domain_map_entry  domain_map [] = {
  {"nc.", cn_domain},
  {"wt.", tw_domain},
  {"kh.", hk_domain},
  {"ku.", uk_domain},
  {"pj.", jp_domain},
  {"rk.", kr_domain},
  {"li.", il_domain},
  {"zn.", nz_domain},
  {"ht.", th_domain},
  {0, 0}
};

#else

#include "ul_url2_inl.cc"

#endif

// ============================================================
void DomainDict::add_top_domain(const char* s) {
  std::string t(s);
  std::reverse(t.begin(), t.end());
  top_domain_.push_back(t);
}

void DomainDict::add_top_country(const char* s) {
  std::string t(s);
  std::reverse(t.begin(), t.end());
  top_country_.push_back(t);
}

void DomainDict::add_general_2nd_domain(const char* s) {
  std::string t(s);
  std::reverse(t.begin(), t.end());
  general_2nd_domain_.push_back(t);
}

void DomainDict::add_country_domain(const char* s, const char* w) {
  std::string t1(s);
  std::reverse(t1.begin(), t1.end());
  std::string t2(w);
  t2 += s;
  std::reverse(t2.begin(), t2.end());
  DomainList& x = country_domain_map_[t1];
  x.push_back(t2);
}

void DomainDict::clear() {
  top_domain_.clear();
  top_country_.clear();
  general_2nd_domain_.clear();
  country_domain_map_.clear();
}

void DomainDict::load_default() {
  for(int i = 0; top_domain[i]; i++) {
    top_domain_.push_back(top_domain[i]);
  }
  for(int i = 0; top_country[i]; i++) {
    top_country_.push_back(top_country[i]);
  }
  for(int i = 0; general_2nd_domain[i]; i++) {
    general_2nd_domain_.push_back(general_2nd_domain[i]);
  }
  for(int i = 0; domain_map[i].top_domain; i++) {
    const char* x = domain_map[i].top_domain;
    for(int j = 0; domain_map[i].second_domain[j]; j++) {
      const char* y = domain_map[i].second_domain[j];
      DomainList& z = country_domain_map_[x];
      z.push_back(y);
    }
  }
}

// 可以存在空行，前后允许出现空格，中间以空格或者是\t分割，可以有多余空格和\t，不支持注释。
// 下面为了方便后面//为主注释，但是文件格式本身不支持注释
// td xxx // td(top_domain),然后定义xxx为一个顶级域名
// tc xxx // tc(top_country),然后定义xxx为一个顶级国家域名
// 2nd xxx // 定义xxx为通用二级域名
// cd xxx yyy // cd(country domain),定义xxx国家域名下面的yyy二级域名。

// 下面是一个example.
// ============================
//         td .org
//         td .com
//         tc .cn
//         tc .uk
//         2nd .net
//         2nd .com
//         cd .cn .bj
//         cd .cn .jx

bool DomainDict::readline(const std::string& str) {
  std::stringstream buf(str);
  std::string type;
  std::string s, s2;
  buf >> type;
  if(type == "td") {
    buf >> s;
    if(s.size()) {
      add_top_domain(s.c_str());
      return true;
    }
    return false;
  } else if(type == "tc") {
    buf >> s;
    if(s.size()) {
      add_top_country(s.c_str());
      return true;
    }
    return false;
  } else if(type == "2nd") {
    buf >> s;
    if(s.size()) {
      add_general_2nd_domain(s.c_str());
      return true;
    }
    return false;
  } else if(type == "cd") {
    buf >> s >> s2;
    if(s.size() && s2.size()) {
      add_country_domain(s.c_str(), s2.c_str());
      return true;
    }
    return false;
  } else if(type == "") { // 支持空行
    return true;
  } else if(type == "#") { // 支持注释
    return true;
  }
  return false;
}

int DomainDict::load_file(const char* path, const char* file_name) {
  // check parameter
  if((path == NULL) ||
      (file_name == NULL) ||
      (file_name[0] == 0)) {
    return ERROR_PARAM;
  }
  std::string file(path);
  if(path[0] == 0) {
    file.append(file_name);
  } else {
    file.append("/").append(file_name);
  }
  std::ifstream in;
  in.open(file.c_str());
  // whether it's opened ok.
  if(!in.is_open()) {
    return OPEN_FILE_FAILED;
  }
  std::string buf;
  while(getline(in, buf)) {
    if(!readline(buf)) {
      in.close();
      return FILE_FORMAT_ERROR;
    }
  }
  in.close();
  return SUCCESS;
}

// ============================================================

static inline const char* rev_match(const char* str, int len, const char* pattern) {
  while(len > 0 && *pattern != 0) {
    len--;
    if(str[len] != *pattern) {
      break;
    }
    pattern++;
  }
  if(*pattern == 0) {
    return str + len;
  } else {
    return 0;
  }
}

static inline const char* rev_dot(const char* str, const char* dot) {
  dot--;
  while(dot > str) {
    if(*dot == '.') {
      return dot + 1;
    }
    dot--;
  }
  return 0;
}

static const char* get_last_two_parts(const char* url) {
  if(url == 0) {
    return url;
  }
  int urllen = strlen(url);
  int dotcount = 0;
  for(int i = urllen - 1; i >= 0; --i) {
    if(*(url + i) == '.') {
      dotcount++;
    }
    if(dotcount == 2) {
      return (url + i + 1);
    }
  }
  return 0;
}

static const char* get_domain_from_site(const DomainDict* dict, const char* site) {
  const char* result = NULL;
  if(site == 0)
    return 0;
  int sitelen = strlen(site);
  if(sitelen == 0) {
    return 0;
  }
  const DomainDict::DomainList& t_top_domain = dict->get_top_domain();
  for(size_t i = 0; i < t_top_domain.size(); i++) {
    const char* match;
    match = rev_match(site, sitelen, t_top_domain[i].c_str());
    if(match != 0) {
      // 以顶级通用域名结尾，将倒数第二节开始的部分作为域名
      result = rev_dot(site, match);
      // 如果不足两节，则将整个域名部分作为主域名
      if(result == 0) {
        return site;
      } else {
        return result;
      }
    }
  }
  const DomainDict::DomainListMap& t_domain_map = dict->get_country_domain_map();
  for(DomainDict::DomainListMap::const_iterator it = t_domain_map.begin();
      it != t_domain_map.end(); ++it) {
    // 以特殊的几个国家域名结尾，包括cn,tw,hk,uk,jp,il,nz,kr
    const char* match;
    match = rev_match(site, sitelen, it->first.c_str());
    if(match != 0) {
      const DomainDict::DomainList& t_second_domain = it->second;
      for(size_t i = 0; i < t_second_domain.size(); i++) {
        const char* match2;
        match2 = rev_match(site, sitelen, t_second_domain[i].c_str());
        if(match2 != 0) {
          // 位置在二级域名列表中，故以倒数第三节开始的部分作为域名
          result = rev_dot(site, match2);
          if(result == 0) {
            // 如果出错，则返回整个site
            return site;
          } else {
            return result;
          }
        }
      }
      goto next;
      // (zhangyan04):2012-1-4:如果区域匹配不上的话，那么继续尝试通用二级域名.
//       // 不在二级域名列表中，将后两节作为域名
//       result = rev_dot(site, match);
//       // 如果不足两节，则将整个域名部分作为主域名
//       if(result == 0) {
//         return site;
//       } else {
//         return result;
//       }
    }
  }
next:
  const DomainDict::DomainList& t_top_country = dict->get_top_country();
  const DomainDict::DomainList& t_general_2nd_domain = dict->get_general_2nd_domain();
  for(size_t i = 0; i < t_top_country.size(); i++) {
    // 以国家域名结尾
    const char* match;
    match = rev_match(site, sitelen, t_top_country[i].c_str());
    if(match != 0) {
      for(size_t j = 0; j < t_general_2nd_domain.size(); j++) {
        const char* match2;
        match2 = rev_match(site, match - site, t_general_2nd_domain[j].c_str());
        if(match2 != 0) {
          // 倒数第二节位于general_2nd_domain中，将后三节作为域名
          result = rev_dot(site, match2);
          if(result == 0) {
            // 如果出错，则返回整个site
            return site;
          } else {
            return result;
          }
        }
      }
      // 不在二级域名列表中，将后两节作为域名
      result = rev_dot(site, match);
      // 如果不足两节，则将整个域名部分作为主域名
      if(result == 0) {
        return site;
      } else {
        return result;
      }
    }
  }
  // 既不以通用域名结尾，也不以国家域名结尾，取后两节作为域名，若不足两节，则以整个串作为域名
  return get_last_two_parts(site);
}

static inline int get_trunk_from_domain(const char* domain,
                                        char* trunk,
                                        int trunk_size) {
  int size = 0;
  while(domain[size] != '.') {
    size++;
  }
  if(size >= trunk_size) {
    return -1;
  }
  memcpy(trunk, domain, size);
  trunk[size] = '\0';
  return 0;
}

// 会判断16进制
static bool is_legal_char(char ch) {
  // 因为包含了16进制的内容
  if(isdigit(ch) || ch == '.' || ch == 'x' || ch == 'X' ||
      (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F')) {
    return true;
  }
  return false;
}


// 初步判断是否为IP
// 对于cc.cc.cc.cc这个就没有办法判断
// 这个明显不是IP但是使用这个函数会认为是IP
// 所以之后我们还要跟进一步判断.
static int guess_is_dotip(const char* site) {
  int dot_count = 0;
  while(*site) {
    if(!is_legal_char(*site)) {
      return 0;
    }
    if(*site == '.') {
      dot_count++;
    }
    site++;
  }
  if(dot_count == 3) {
    return 1; // 是IP,但是实际也可能不为IP.
  }
  return 0; // 不是IP
}

// 只是判断10进制
static bool is_legal_char2(char ch) {
  if(isdigit(ch) || ch == '.') {
    return true;
  }
  return false;
}

static int guess_is_dotip2(const char* site) {
  int dot_count = 0;
  while(*site) {
    if(!is_legal_char2(*site)) {
      return 0;
    }
    if(*site == '.') {
      dot_count++;
    }
    site++;
  }
  if(dot_count == 3) {
    return 1; // 是IP,但是实际也可能不为IP.
  }
  return 0; // 不是IP
}

static inline int get_value(const char* p, const char endc) {
  int val = 0;
  if(p[0] == '0' && (p[1] == 'x' || p[1] == 'X')) {
    p += 2;
    while(*p != endc) {
      char ch = *p;
      if(ch >= '0' && ch <= '9') {
        ch -= '0';
      } else if(ch >= 'a' && ch <= 'f') {
        ch -= 'a';
        ch += 10;
      } else if(ch >= 'A' && ch <= 'F') {
        ch -= 'A';
        ch += 10;
      } else {
        return -1;
      }
      val = val * 16 + ch;
      p++;
      //这个部分不是一个合法子网
      if(val >= 256) {
        return ILLEGAL_IP;
      }
    }
  } else if(p[0] == '0') { // oct.
    p += 1;
    while(*p != endc) {
      char ch = *p;
      if(ch >= '0' && ch <= '7') {
        ch -= '0';
      } else {
        return -1;
      }
      val = val * 8 + ch;
      p++;
      //这个部分不是一个合法子网
      if(val >= 256) {
        return ILLEGAL_IP;
      }
    }
  } else { // dec.
    while(*p != endc) {
      char ch = *p;
      if(ch >= '0' && ch <= '9') {
        ch -= '0';
      } else {
        return -1;
      }
      val = val * 10 + ch;
      p++;
      //这个部分不是一个合法子网
      if(val >= 256) {
        return ILLEGAL_IP;
      }
    }
  }
  return val;
}

// 判断这个site是否可能为ip，如果是ip的话我们需要进行一次归一化
int ul_normalize_site_ip(const char* site,
                         char* result,
                         int result_size) {
  char tmp[256]; // I think it's enough.
  const char* saved_site = site;
  if(guess_is_dotip(site)) {
    int offset = 0;
    while(1) {
      const char* p = strchr(site, '.');

      int val = 0;
      if(!p) { // last part.
        val = get_value(site, '\0');
        if(val == -1) {
          goto not_ip;
        } else if(val == ILLEGAL_IP) {
          return ILLEGAL_IP;
        }
        offset += snprintf(tmp + offset, sizeof(tmp) - offset, "%d", val);
        break;
      } else {
        val = get_value(site, '.');
        if(val == -1) {
          goto not_ip;
        } else if(val == ILLEGAL_IP) {
          return ILLEGAL_IP;
        }
        offset += snprintf(tmp + offset, sizeof(tmp) - offset, "%d.", val);
        site = p + 1;
      }
    }
    saved_site = tmp;
  }
not_ip:
  // same as domain
  int ret = snprintf(result, result_size, "%s", saved_site);
  if(ret < 0 || ret >= result_size) {
    return SITE_BUFFER_SMALL;
  }
  return SUCCESS;
}

static int ul_fetch_trunk_and_domain(const DomainDict* dict,
                                     const char* site,
                                     const char** domain,
                                     char* trunk,
                                     int trunk_size) {
  // 如果是IP
  if(guess_is_dotip2(site)) {
    *domain = site;
    // 如果提供trunk的话
    if(trunk) {
      int ret = snprintf(trunk, trunk_size, "%s", site);
      if(ret < 0 || ret >= trunk_size) {
        return TRUNK_BUFFER_SMALL;
      }
    }
    return SUCCESS;
  }

  // 如果不是IP
  const char* tmp_domain = get_domain_from_site(dict, site);
  // 忽略最前面的'.'
  while(tmp_domain && *tmp_domain=='.'){
    tmp_domain++;
  }
  if(tmp_domain) {    
    *domain = tmp_domain;
    // 如果提供trunk的话
    if(trunk) {
      if(get_trunk_from_domain(tmp_domain, trunk, trunk_size) != 0) {
        return TRUNK_BUFFER_SMALL;
      }
    }
    // OK.
    return SUCCESS;
  } else {
    // 获取domain存在问题
    return ILLEGAL_DOMAIN;
  }
}

const char* ul_fetch_maindomain_ex(const char* site, char* trunk, int trunk_size,
                                   bool recoveryMode) {
  const char* domain = 0;
  static DomainDict dict;
  static bool init = false;
  if(!init) {
    dict.load_default();
    init = true;
  }
  if(ul_fetch_trunk_and_domain(&dict, site, &domain, trunk, trunk_size) != SUCCESS) {
    if(recoveryMode) {
      strncpy(trunk, site, trunk_size - 1);
      trunk[trunk_size-1] = 0;
      return site;
    }
    return NULL;
  }
  return domain;
}

const char* ul_fetch_maindomain_ex2(const char* site, char* trunk, int trunk_size,
                                    const DomainDict* dict, bool recoveryMode) {
  if(dict == NULL) {
    return ul_fetch_maindomain_ex(site, trunk, trunk_size);
  }
  const char* domain = 0;
  if(ul_fetch_trunk_and_domain(dict, site, &domain, trunk, trunk_size) != SUCCESS) {
    if(recoveryMode) {
      strncpy(trunk, site, trunk_size - 1);
      trunk[trunk_size-1] = 0;
      return site;
    }
    return NULL;
  }
  return domain;
}


#if 0

int main() {
  printf("# top domain\n");
  for(int i = 0; top_domain[i]; i++) {
    std::string s(top_domain[i]);
    reverse(s.begin(), s.end());
    printf("td %s\n", s.c_str());
  }
  printf("\n");

  printf("# top country\n");
  for(int i = 0; top_country[i]; i++) {
    std::string s(top_country[i]);
    reverse(s.begin(), s.end());
    printf("tc %s\n", s.c_str());
  }
  printf("\n");

  printf("# general 2nd domain\n");
  for(int i = 0; general_2nd_domain[i]; i++) {
    std::string s(general_2nd_domain[i]);
    reverse(s.begin(), s.end());
    printf("2nd %s\n", s.c_str());
  }
  printf("\n");

  printf("# country domain\n");
  for(int i = 0; domain_map[i].top_domain; i++) {
    std::string s1(domain_map[i].top_domain);
    reverse(s1.begin(), s1.end());
    const char** p = domain_map[i].second_domain;
    for(int j = 0; p[j]; j++) {
      std::string s2(p[j]);
      s2 = s2.slcstr(s1.length());
      reverse(s2.begin(), s2.end());
      printf("cd %s %s\n", s1.c_str(), s2.c_str());
    }
  }
  printf("\n");
  return 0;
}

#endif // #ifdef 0

