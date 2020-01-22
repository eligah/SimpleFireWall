#include <iostream>
#include <string>
#include <set>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <fstream>
#include <memory>

using namespace std;
//route to port - ip table for different protocal and direction
#define TCP 0
#define UDP 1
#define IN 0
#define OUT 1

//Interval tree implementation with shared_ptr
//see: https://en.wikipedia.org/wiki/Interval_tree
typedef pair<uint32_t, uint32_t> Interval;
struct ITNode
{
    Interval i;
    int max;
    shared_ptr<ITNode> left, right;
    ITNode(Interval i_)
    {
        i = i_;
        max = i.second;
        left = nullptr, right = nullptr;
    }
};

shared_ptr<ITNode> insert(shared_ptr<ITNode>& root, Interval i)
{
    if (!root)
        return make_shared<ITNode>(i);

    int l = root->i.first;
    if (i.first < l)
        root->left = insert(root->left, i);
    else
        root->right = insert(root->right, i);
    if (root->max < i.second)
        root->max = i.second;
    return root;
}

//Helper function to decide the intersection of two intervals
bool doOVerlap(Interval i1, Interval i2)
{
    return i1.first <= i2.second && i2.first <= i1.second;
}

bool findInterval(shared_ptr<ITNode>& root, Interval& i)
{
    if (!root) return false;
    if (doOVerlap(root->i, i))
        return true;
    if (root->left && root->left->max >= i.first)
        return findInterval(root->left, i);
    return findInterval(root->right, i);
}

class FireWall
{
    //Map specific port to a series of ip intervals
    using IPPORT = unordered_map<int, shared_ptr<ITNode>>;
public:
    FireWall(const string& file_path);
    bool accept_packet(const string& direction, const string& protocol,
                       int port, const string& address);
private:
    vector<IPPORT> hash;//4 port-ip tables: 0:OUT_UDP, 1:OUT_TCP, 2:IN_UDP, 3:IN_TCP
    inline IPPORT& getRulesTable(const string& dir, const string& protocol) //route to port-ip table
    {
        int idx = 2 * (dir == "inbound" ? IN : OUT) + (protocol == "tcp" ? TCP : UDP);
        return hash[idx];
    }
    uint32_t toUInt(const string& ip); //convert ip to uint_32
};

FireWall::FireWall(const string& file_path)
{
    hash = vector<IPPORT>(4, unordered_map<int, shared_ptr<ITNode>>());
    ifstream in(file_path);
    string direction, protocol, address, port, line;
    while (in.good())
    {
        getline(in, direction, ',');
        getline(in, protocol, ',');
        getline(in, port, ',');
        getline(in, address, '\n');

        //parse port and ip with interval
        string port1, port2;
        size_t it = port.find('-');
        if (it != string::npos)
        {
            port1 = port.substr(0, it);
            port2 = port.substr(it + 1);
        }
        else port1 = port2 = port;
        it = address.find('-');
        string ip1, ip2;
        if (it != string::npos)
        {
            ip1 = address.substr(0, it);
            ip2 = address.substr(it + 1);
        }
        else ip1 = ip2 = address;

        pair<uint32_t, uint32_t> tmp{ toUInt(ip1), toUInt(ip2) };
        auto& rulesTable = getRulesTable(direction, protocol);
        uint32_t port1U = stoi(port1), port2U = stoi(port2);

        //Insert the interval from port1U to port2U
        for (auto i = port1U; i <= port2U; i++)
            rulesTable[i] = insert(rulesTable[i], tmp);
    }
}

uint32_t FireWall::toUInt(const string& ip)
{
    uint32_t a, b, c, d;
    char ch;
    stringstream  ss(ip);
    ss >> a >> ch >> b >> ch >> c >> ch >> d;
    return (a << 24) | (b << 16) | (c << 8) | d;
}

bool FireWall::accept_packet(const string& direction, const string& protocol,
                             int port, const string& address)
{
    auto& rulesTable = getRulesTable(direction, protocol);
    if (rulesTable.count(port) == 0) return false;
    auto ips = rulesTable[port];
    Interval tmp{ toUInt(address), toUInt(address) };
    return findInterval(ips, tmp);
}


void test(FireWall& fw, string test_path)
{
    ifstream in(test_path);
    string direction, protocol, address, port, res;
    int cnt = 1;
    while (in.good()) {
        getline(in, direction, ',');
        getline(in, protocol, ',');
        getline(in, port, ',');
        getline(in, address, ',');
        getline(in, res, '\n');
        cout << "testing " << cnt << " "  << direction << " " << protocol << " " << port <<" " << address;
        bool testRes = fw.accept_packet(direction,protocol,stoi(port),address);
        if((res == "1" && testRes == true )|| (res == "0" && testRes == false))
            cout << " passed" << endl;
        else
            cout <<" failed" << endl;
        cnt++;
    }
}

int main()
{
    FireWall fw("./fw.csv");
    test(fw, "./test.csv");
    return 0;
}
