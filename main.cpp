#include <iostream>
#include <map>
#include <random>
#include <chrono>
#include <string>
#include <set>
#include <stdexcept>
#include <fstream>
#include <vector>

#define ARG_ERROR {throw invalid_argument("you take incorect arguments for your user_type"); return{};}

using namespace std;

static mt19937_64 gen{static_cast<unsigned long long>(chrono::system_clock::now().time_since_epoch().count())};

template<typename  T>
ostream& operator << (ostream& out, vector<T> vec_){
    bool comma = false;

    out << "(";
    for(const auto& a: vec_){
        if(comma) out << ", ";
        else comma = true;

        out << a;
    }
    out << ")" << endl;

    return out;
}


set<int> chose_rand_k_position(int k, int n){ //ВОЗВРАЩАЕТ k РАНДОМНЫХ ПОЗИЦИЙ ИЗ n
    if(k > n) throw invalid_argument("you can't chose k = " + to_string(k) + " diferent position from n = " + to_string(n) + " elements");
    if(k == 0) return {};

    set<int> result;

    //с каждом шагом диапозон значений для random снижается на единичку (будем "вычеркивать" выбраную позицию из quantity_delets_elems)
    vector<int> quantity_delets_elems(n, 0); //показывет сколько позицей уже выбрано до i-го элемента

    for(int selection_range = n; selection_range > n - k; selection_range--){
        uniform_int_distribution<int> rnd(0, selection_range-1);

        int chose_relative = rnd(gen);

        //какой элемент мы действительно выбрали, с учетом "вычеркнутых"
        result.insert(chose_relative + quantity_delets_elems[chose_relative]);

        //вычеркиваем chose_relative из quantity_delets_elems и увеличиваем значение всех следующих за chose_relative элементов на 1
        for(int j = chose_relative; j < selection_range-1; j++){
            quantity_delets_elems[j] = quantity_delets_elems[j+1] + 1;
        }

    }

    return result;
}

enum class UserType{
    Typography,
    AdvertisyAgence,
    Common
};

struct TelephoneInfo{
    string tel;
    string additional;
};

bool operator == (const TelephoneInfo& lhs, const TelephoneInfo& rhs){
    return ((lhs.tel == rhs.tel) && (lhs.additional == rhs.additional));
}

ostream& operator << (ostream& out, multimap<string, TelephoneInfo> map_){
    bool comma = false;

    for(const auto& [k, v] : map_){
        if(comma) out << ", " << endl;
        else comma = true;

        out << "[" << k << string(10 - k.size(), ' ') << ": " << v.tel << string (8 - v.tel.size(),' ') << "(" << v.additional << ")]";
    }
    out << ".";
    return out;
}

class TelephoneDirectori{
public:
    using DataType = multimap<string, TelephoneInfo>;

    template<typename ... Types>
    auto take_info(UserType user, Types ... args){
        switch (user) {
            case UserType::Typography:
                return take_info_Typography(args ...);

            case UserType::AdvertisyAgence:
                return take_info_AdvertisyAgence(args ...);

            case UserType::Common:
                return take_info_Common(args ...);
        }
    }

    void add(pair<string, TelephoneInfo> person){
        auto [begin_range, end_range] = database.equal_range(person.first);

        for(auto it = begin_range; it != end_range; ++it)
            if(person.second == it->second) return; //не добавляем полностью индентичного человека

        database.insert(person);
    }

private:
    const DataType& take_info_Typography(){
        return database;
    }

    DataType take_info_AdvertisyAgence(int k){
        if(k == 0) return {};

        set<int> selected_position = chose_rand_k_position(k, database.size());
        DataType result;

        int count = 0;
        auto it_chose_pos = selected_position.begin();
        for(auto it = database.begin(); it != database.end(); ++it, count++)
            if(count == *it_chose_pos) {
                result.insert(*it);

                ++it_chose_pos;
                if(it_chose_pos == selected_position.end()) break;
            }

        return result;
    }

    DataType take_info_Common(const string& search){
        auto [begin_copy, end_copy] = database.equal_range(search);

        return DataType(begin_copy, end_copy);
    }



    const DataType&  take_info_AdvertisyAgence() ARG_ERROR
    const DataType&  take_info_Common() ARG_ERROR
    DataType take_info_Typography(string a) ARG_ERROR
    DataType take_info_AdvertisyAgence(string a) ARG_ERROR
    DataType take_info_Typography(int a) ARG_ERROR
    DataType take_info_Common(int a) ARG_ERROR

    DataType database;
};

vector<pair<string, TelephoneInfo>> parse_file(){
    ifstream in("names_data.txt");

    int n;
    in >> n;
    //cout << n;

    vector<pair<string, TelephoneInfo>> result;
    result.reserve(n);

    vector<string> intermediate_first(n);
    vector<string> intermediate_second(n);

    for(int i = 0; i < n; i++){
        in >> intermediate_first[i] >> intermediate_second[i];
        //cout << intermediate_first[i]<<intermediate_second[i] << endl;
    }

    int tel;
    for(int i = 0; i < n; i++){
        in >> tel;
        string view_like_tel = to_string(tel/1000) + "-" + to_string((tel/100)%10) + "-" + to_string(tel%100);
        result.push_back({intermediate_first[i], TelephoneInfo{view_like_tel,intermediate_second[i]}});
    }

    return result;
}

int main() {
    TelephoneDirectori book;

    vector<string> names = {};

    vector<pair<string,TelephoneInfo>> peoples = parse_file();

    for(const auto& person : peoples) book.add(person);

    UserType T = UserType::Typography;
    UserType A = UserType::AdvertisyAgence;
    UserType C = UserType::Common;


    cout << book.take_info(T);


    return 0;
}
