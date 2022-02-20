#include <bits\stdc++.h>
using namespace std;
const int MAXN = 100;
//=============BF================================
/*
method: BF
*/
char* BF(char *str, char *str_pattern){
    return strstr(str, str_pattern);
}
//==============KMP===============================
// The algorithm was conceived by James H. Morris and independently discovered by Donald Knuth "a few weeks later" from automata theory.Morris and Vaughan Pratt published a technical report in 1970.The three also published the algorithm jointly in 1977.Independently, in 1969, Matiyasevich discovered a similar algorithm, coded by a two-dimensional Turing machine, while studying a string-pattern-matching recognition problem over a binary alphabet. This was the first linear-time algorithm for string matching.
// 对前缀的利用
/*
method: KMP
function: KMP_next数组求解（预处理）
Param:
    @p      模式串
    @nxt    next数组
    @plen   模式串长度
*/
int nxt[MAXN];
void getNXT(string p){
    int plen = p.length();
    int i = 0;
    int j = nxt[0] = -1;
    while(i<plen){
        while(j!=-1 && p[i]!=p[j]){
            //匹配失败或无相同前后缀时
            j = nxt[j];
        }
        nxt[++i] = ++j;
    }
}

/*
method: KMP
function: KMP匹配算法
Param:
    @pLen   模式串长度
    @sLen   文本长度
    @s      文本串
    @p      模式串
    @nxt[]  next数组
    @ans    匹配的串数量
*/
int kmp(string p, string s)
{
    int pLen = p.length();
    int sLen = s.length();
    int i, j, ans;
    i = j = ans = 0;
    getNXT(p);
    while(i<sLen){
        // 若无法匹配，移动使得前后缀重叠，当无相同前后缀时结束
        while(j!=-1 && s[i]!=p[j])
            j = nxt[j];
        ++i, ++j;
        if(j>=pLen){
            ans++;
            //cout << i - j << endl;
            j = nxt[i];
        }
    }
    return ans;
}  

void test_KMP(){
    string s = "abcacabcbcbacabc", p = "cbacabc";
    getNXT(p);
    cout << kmp(p, s);
    return;
}

// 应用      
// 字符串中本质不同子串的数目---添加一个字符后: number = abs(s) + 1 - max(nxt)
// 字符串压缩: k = n - nxt[n-1]
// 前缀函数构建自动机

//===========================Boyer-Moore=============================
// In computer science, the Boyer–Moore string-search algorithm is an efficient string-searching algorithm that is the standard benchmark for practical string-search literature.[1] It was developed by Robert S. Boyer and J Strother Moore in 1977.[2] The original paper contained static tables for computing the pattern shifts without an explanation of how to produce them. The algorithm for producing the tables was published in a follow-on paper; this paper contained errors which were later corrected by Wojciech Rytter in 1980.[3][4] The algorithm preprocesses the string being searched for (the pattern), but not the string being searched in (the text). It is thus well-suited for applications in which the pattern is much shorter than the text or where it persists across multiple searches. The Boyer–Moore algorithm uses information gathered during the preprocess step to skip sections of the text, resulting in a lower constant factor than many other string search algorithms. In general, the algorithm runs faster as the pattern length increases. The key features of the algorithm are to match on the tail of the pattern rather than the head, and to skip along the text in jumps of multiple characters rather than searching every single character in the text.
// 通过后缀匹配获得比前缀匹配更多的信息
/*
method: BM
function: Delta1_arr
Param:
    @pattern    模式串
    @bmBc       Delta1_arr
    @m          模式串长度
*/
void Delta1(char *pattern, int m, int bmBc[]){
    //一个字符占八位，共256个字符
    //初始化：将所有字符失配时的移动距离都赋值为m
    for (int i = 0; i < 256; i++){
        bmBc[i] = m;
        //bmBc[i] = m + 1;
    }
    //针对模式串pattern中存在的每一个字符
    //计算出它们最靠右的（非最后一个字符）地方距离串末尾的距离
    //即它们失配时该移动的距离
    for (int i = 0; i < m - 1; i++){
        bmBc[pattern[i]] = m - 1 - i;
        //bmBc[pattern[i]] = m - i;
    }
}

/*
method: BM
funciton: Delta2_rpr
Param:
    @pattern    模式串
    @m          模式串长度
    @suf        后缀子串为前缀子串
    @pre        
*/
void suffix(char *pattern, int m, int suf[], bool pre[]){
    int i, j, k;
    for(i = 0; i < m; ++i)//两个数组初始化
    {
        suf[i] = -1;
        pre[i] = false;
    }
    for(i = 0; i < m-1; ++i)//b[0,i]
    {
        j = i;
        k = 0;//公共后缀子串长度(模式串尾部取k个出来，分别比较)
        while(j >= 0 && pattern[j] == pattern[m-1-k])//与b[0,m-1]求公共后缀子串
        {
            --j;
            ++k;
            suf[k] = j+1;
            //相同后缀子串长度为k时，该子串在b[0,i]中的起始下标
            // (如果有多个相同长度的子串，被赋值覆盖，存较大的)
        }
        if(j == -1)//查找到模式串的头部了
            pre[k] = true;//如果公共后缀子串也是模式串的前缀子串
    }
}

/*
method: BM
function: Delta2
Param:
    @j      坏字符对应的模式串中字符下标
    @m      模式串长度
    @suf    后缀子串为前缀子串
    @pre        
*/
int Delta2(int j, int m, int *suf, bool *pre){
    int k = m - 1 - j;//好后缀长度
    if (suf[k] != -1)               //case1: 找到与后缀相同子串
        return j - suf[k] + 1;
    for (int r = j + 2; r<m; ++r)   //case2: 检查后缀的子串
    {
        if(pre[m-r]==true)
            return r;
    }
    return m;                       //case3: 直接移动m位
}

/*
method: BM
function: BM
Param:
    @j      坏字符对应的模式串中字符下标
    @m      模式串长度
    @suf    后缀子串为前缀子串
    @pre        
*/
int BM(char *str, int n, char *pattern, int m){
    int *bmBc = new int[256];
    int *suf = new int[m];
    bool *pre = new bool[m];
    Delta1(pattern, m, bmBc);
    suffix(pattern, m, suf, pre);
    int move_1, move_2, i, j;
    while (i < n - m + 1)
    {
        for (j = m - 1; j >= 0; --j){
            if(str[i+j] != pattern[j])
                break;
        }
        if(j<0){
            delete[] bmBc;
            delete[] suf;
            delete[] pre;
            
            return i;
        }
        move_1 = bmBc[int(str[i + j])];
        move_2 = 0;
        if(j < m-1){
            move_2 = Delta2(j, m, suf, pre);
        }
        i = i + max(move_1, move_2);
    }
    delete[] bmBc;
    delete[] suf;
    delete[] pre;
    return -1;
}

void test_BM()
{
    string a = "abcacabcbcbacabc", b = "cbacabc";
    cout << a << " " << b << " " << BM(&a[0],a.size(),&b[0],b.size());
    return;
}

//=====================Sunday=============================
// 实际上只修改了Delta1，拓展了一位
/*
method: Sunday
function: Delta1_arr
Param:
    @pattern    模式串
    @bmBc       Delta1_arr
    @m          模式串长度
*/
void Delta1_Sunday(char *pattern, int m, int bmBc[]){
    //一个字符占八位，共256个字符
    //初始化：将所有字符失配时的移动距离都赋值为m
    for (int i = 0; i < 256; i++){
        //bmBc[i] = m;
        bmBc[i] = m + 1;
    }
    //针对模式串pattern中存在的每一个字符
    //计算出它们最靠右的（非最后一个字符）地方距离串末尾的距离
    //即它们失配时该移动的距离
    for (int i = 0; i < m - 1; i++){
        //bmBc[pattern[i]] = m - 1 - i;
        bmBc[pattern[i]] = m - i;
    }
}


/*
function:Sunday字符匹配算法
Param:
@T 文本内容
@lenT文本内容长度
@p 需要匹配的字符串
@lenP 需要匹配的字符串长度
@sun_shift 最终存放每个字符失配时该右移的距离
*/
char* Sunday(char* T,int lenT,char* P,int lenP,int shift[]){
	int i;
    int pos=0;
    int j;
    while(pos<=lenT-lenP) {
        j=0;
        while(T[pos+j]==P[j]&&j<lenP) j++;
        if(j>=lenP){
            return &T[pos];//匹配成功，返回地址
            cout << pos << endl;
        }
        else{
 
			pos+=shift[T[pos+lenP]];
		}
    }
    return NULL;
}

void test_Sunday(){
    string a = "abcacabcbcbacabc", b = "cbacabc";
    int bmBc[256];
    Delta1_Sunday(&b[0], b.size(), bmBc);
    cout << Sunday(&a[0],a.size(),&b[0],b.size(), bmBc);
    return;
}

//=======================Horspool==========================
/*
function:horspool字符匹配算预处理
Param:
@P 需要匹配的字符串
@lenP 需要匹配的字符串长度
*/
void horspoolPre(int hors_d[],char* P,int lenP)
{
    int i;
    for(i=0;i<256;i++){
        hors_d[i]=lenP;
    }
    for(i=0;i<(lenP-1);i++){
        hors_d[P[i]]=lenP-i-1;
    }
}
/*
function:horspool字符匹配算法
Param:
@T 文本内容
@lenT 文本内容长度
@P 需要匹配的字符串
@lenP 需要匹配的字符串长度
@hors_d[] 辅助数组
*/
char* horspool(char *T, int lenT, char *P, int lenP,int hors_d[])  
{  
    int i,pos,j; 
    pos=0;  
    while(pos<=(lenT-lenP)){  
        j=lenP-1;  
        while(j>=0&&T[pos+j]==P[j]) j--;  
        if(j==-1){
            return &T[pos];
		}			 
        else{  
            pos+=hors_d[T[pos+lenP-1]];
		}
    }  
    return NULL;  
}

//https://en.wikipedia.org/wiki/Knuth%E2%80%93Morris%E2%80%93Pratt_algorithm
//https://en.wikipedia.org/wiki/Boyer%E2%80%93Moore_string-search_algorithm
//https://en.wikipedia.org/wiki/Rabin%E2%80%93Karp_algorithm
//https://blog.csdn.net/qq_33515733/article/details/81163135?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522164532397816780366576310%2522%252C%2522scm%2522%253A%252220140713.130102334..%2522%257D&request_id=164532397816780366576310&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~baidu_landing_v2~default-1-81163135.first_rank_v2_pc_rank_v29&utm_term=BM+Sunday&spm=1018.2226.3001.4187
//https://blog.csdn.net/qq_21201267/article/details/92799488?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522164532168516780366542579%2522%252C%2522scm%2522%253A%252220140713.130102334..%2522%257D&request_id=164532168516780366542579&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~sobaiduend~default-3-92799488.first_rank_v2_pc_rank_v29&utm_term=BM&spm=1018.2226.3001.4187
//https://oi-wiki.org/string/bm/#boyer-moore-sunday
