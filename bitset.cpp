#include <vector>
#include <cstring>
#include <cstdint>
#include <iostream>
#include <algorithm>
typedef unsigned long long ull;
struct dynamic_bitset {
    const static size_t base = 64;
    ull full[base+1],power[base+1];
    std::vector <ull> bits;
    size_t Size;

    void init(){
        power[0]=1;
        for (size_t i = 1; i < base ; i++)
            power[i]=power[i-1]*2;
        full[0] = 0;
        for (size_t i = 1; i <= base ; i++)
            full[i]= full[i - 1] * 2 + 1;
    }
    // 默认构造函数，默认长度为 0
    dynamic_bitset() {
        init();
        bits = {};
        Size = 0;
    }

    ~dynamic_bitset() = default;

    dynamic_bitset(const dynamic_bitset &) = default;

    dynamic_bitset &operator = (const dynamic_bitset &) = default;

    dynamic_bitset(std::size_t n){
        init();
        Size = n;
        if (n == 0) return;
        for (size_t i = 1; i <= ( n - 1 ) / base + 1; i++)
            bits.push_back(0);
    }

    dynamic_bitset(const std::string &str){
        init();
        bits.clear();
        if (str.empty()){
            Size = 0;
            return;
        }
        Size = str.length();
        ull tmp = 0;
        size_t cnt = 0;
        for (size_t i = 0; i < Size; i++){
            tmp += (str[i] - '0') * power[cnt];
            cnt++;
            if (cnt == base) {
                cnt = 0;
                bits.push_back(tmp);
                tmp = 0;
            }
        }
        if (cnt > 0){
            bits.push_back(tmp);
        }
    }
    // 访问第 n 个位的值，和 vector 一样是 0-base
    bool operator [] (std::size_t n) const{
        ull pos1 = (n / base), pos2 = (n % base);
        return (bits[pos1]>>pos2) & 1;
    }
    // 把第 n 位设置为指定值 val
    dynamic_bitset &set(std::size_t n, bool val = true){
        size_t pos1 = (n / base), pos2 = (n % base);
        if (((bits[pos1]>>(pos2)) & power[0]) != val)
            bits[ pos1 ] = bits[ pos1 ] ^ power[pos2];
        return (*this);
    }
    // 在尾部插入一个位，并且长度加一
    /*
    补充说明：这里指的是高位，
    比如0010后面push_back(1)应该变为00101
    */
    dynamic_bitset &push_back(bool val){
        if (Size % base == 0){
            Size++;
            bits.push_back(val);
        }else{
            bits[bits.size()-1] += val*(power[Size%base]);
            Size++;
        }
        return (*this);
    }

    // 如果不存在 1 ，则返回 true。否则返回 false
    bool none() const {
        if (Size == 0) return true;
        for (size_t i = 0; i < bits.size(); i++)
            if (bits[i] > 0) return false;
        return true;
    }
    // 如果不存在 0 ，则返回 true。否则返回 false
    bool all() const {
        if (Size == 0) return true;
        for (size_t i = 0; i < bits.size()-1; i++)
            if (bits[i] != full[base]) return false;
        if (bits.back() != full[(Size-1) % base + 1]) return false;
        return true;
    }

    // 返回自身的长度
    std::size_t size() const{
        return Size;
    }

    // 或操作，返回自身的引用。     a |= b 即 a = a | b
    dynamic_bitset &operator |= (const dynamic_bitset &other){
        dynamic_bitset self = (*this);
        if (self.Size == 0 || other.Size == 0) return (*this);
        if (self.Size >= other.Size){
            size_t sz = other.bits.size();
            for (size_t i = 0; i < sz; i++)
                self.bits[i] = self.bits[i] | other.bits[i];
        }else{
            size_t sz = self.bits.size();
            for (size_t i = 0; i < sz - 1; i++)
                self.bits[i] = self.bits[i] | other.bits[i];
            self.bits[sz-1] = self.bits[sz-1] | (other.bits[sz-1] & full[(self.Size-1) % base + 1] );
        }
        *this = self;
        return (*this);
    }
    // 与操作，返回自身的引用。     a &= b 即 a = a & b
    dynamic_bitset &operator &= (const dynamic_bitset &other){
        dynamic_bitset self = (*this);
        if (self.Size == 0 || other.Size == 0) return (*this);
        if (self.Size >= other.Size){
            size_t sz = other.bits.size();
            for (size_t i = 0; i < sz - 1; i++)
                self.bits[i] = self.bits[i] & other.bits[i];
            size_t tmp = (other.Size - 1) % base +1;
            self.bits[sz-1] = (self.bits[sz-1] & other.bits[sz-1]) | ((self.bits[sz-1]>>(tmp))<<tmp);
        }else{
            size_t sz = self.bits.size();
            for (size_t i = 0; i < sz - 1; i++)
                self.bits[i] = self.bits[i] & other.bits[i];
            self.bits[sz-1] = self.bits[sz-1] & (other.bits[sz-1] & full[(self.Size-1) % base + 1] );
        }
        *this = self;
        return (*this);
    }
    // 异或操作，返回自身的引用。   a ^= b 即 a = a ^ b
    dynamic_bitset &operator ^= (const dynamic_bitset &other){
        dynamic_bitset self = (*this);
        if (self.Size == 0 || other.Size == 0) return (*this);
        if (self.Size >= other.Size){
            size_t sz = other.bits.size();
            for (size_t i = 0; i < sz; i++)
                self.bits[i] = self.bits[i] ^ other.bits[i];
        }else{
            size_t sz = self.bits.size();
            for (size_t i = 0; i < sz - 1; i++)
                self.bits[i] = self.bits[i] ^ other.bits[i];
            self.bits[sz-1] = self.bits[sz-1] ^ (other.bits[sz-1] & full[(self.Size-1) % base + 1] );
        }
        *this = self;
        return (*this);
    }
    dynamic_bitset &operator <<= (std::size_t n){
        dynamic_bitset self(*this);
        std::vector<ull> ans={};
        size_t ret = ((self.Size-1) % base + 1); //lst
        size_t sz = self.bits.size();
        size_t pos1 = n / base, pos2 = n % base;
        for (size_t i = 1; i <= pos1; i++) ans.push_back(0);

        if (sz == 0){
            if (pos2 > 0) ans.push_back(0);
            self.bits = ans;
            self.Size = n;
            (*this) = self;
            return (*this);
        }

        ull lst = 0;
        for (size_t i = 0; i < sz-1; i++){
            ans.push_back( lst + ((self.bits[i] & full[base-pos2])<<pos2) );
            lst = self.bits[i]>>(base-pos2);
        }
        if (pos2 + ret > base){
            ans.push_back( lst + ((self.bits.back() & full[base-pos2])<<pos2));
            lst = self.bits.back()>>(base-pos2);
            ans.push_back(lst);
        }else{
            ans.push_back( lst + ((self.bits.back() & full[base-pos2])<<pos2));
        }
        self.bits = ans;
        self.Size += n;
        (*this) = self;
        return (*this);
    }
    dynamic_bitset &operator >>= (std::size_t n){
        if (n>=Size){
            Size = 0;
            bits.clear();
            return (*this);
        }
        dynamic_bitset self(*this);
        std::vector<ull> ans={};
        size_t pos1 = n / base, pos2 = n % base;
        size_t sz = self.bits.size();
        size_t ret = (self.Size-1) % base + 1;

        ull lst = (self.bits[pos1] >> pos2);
        for (size_t i = pos1 + 1; i < sz - 1; i++){
            ans.push_back( lst + ((self.bits[i]&full[pos2])<<(base-pos2)) );
            lst = self.bits[i] >> pos2;
        }
        if (ret + (base-pos2) > base){
            ans.push_back(lst + ((self.bits[sz-1]&full[pos2])<<(base-pos2)));
            ans.push_back(self.bits[sz-1]>>(pos2));
        }else{
            ans.push_back(lst + ((self.bits[sz-1]&full[pos2])<<(base-pos2)));
        }
        self.bits = ans;
        self.Size -= n;
        *this = self;
        return (*this);
    }

    // 把所有位设置为 1
    dynamic_bitset &set     (){
        if (Size == 0) return (*this);
        for (size_t i = 0; i < bits.size()-1; i++)
            bits[i] = full[base];
        bits.back() = full[(Size-1) % base + 1];
        return (*this);
    }
    // 把所有位取反
    dynamic_bitset &flip    (){
        if (Size == 0) return (*this);
        for (size_t i = 0; i < bits.size()-1; i++)
            bits[i] ^= full[base];
        bits.back() ^= full[(Size-1) % base + 1];
        return (*this);
    }
    // 把所有位设置为 0
    dynamic_bitset &reset   (){
        if (Size == 0) return (*this);
        for (size_t i = 0; i < (bits.size()); i++)
            bits[i] = 0;
        return (*this);
    }
};




void print(dynamic_bitset &dyn) {
    std::cout << "Size: " << dyn.size() << " | ";
    for (std::size_t i = 0 ; i != dyn.size() ; ++i) std::cout << dyn[i];
    std::cout << '\n';
}

signed main() {
    std::string str = "010101",str2 = "0001";
    for (size_t i =1;i<=5;i++)str2 +=str2;
    dynamic_bitset a(str2),b(str),c;
    print(b);
    b>>=1;
    print(b);
    print(a);
    a>>=0;
    print(a);
    a>>=1;
    print(a);
    c<<=129;
    print(c);
    //b<<=0;print(b);
    //b<<=1;print(b);
    //b<<=5;print(b);
    //b<<=64;print(b);
    //print(b);print(c);
    return 0;
}