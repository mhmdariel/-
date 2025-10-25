//Below is a self-contained C++17 program. Compile with g++ -std=c++17 -O2 masjid_stream_with_manual_approval.cpp -o masjid_stream and run ./masjid_stream. Interrupt with Ctrl+C.
// masjid_stream_with_manual_approval.cpp
// Compile: g++ -std=c++17 -O2 masjid_stream_with_manual_approval.cpp -o masjid_stream
// Run: ./masjid_stream
//
// Safe simulation: continuously generates masjid designs in many mathematical spaces,
// prints full-stack amenities and estimates simulated cost units.
// ANY "spending" is SIMULATED and requires deliberate human typed confirmation.
// This program never connects to the network or performs real-world transactions.

#include <bits/stdc++.h>
#include <thread>
#include <atomic>
#include <chrono>
using namespace std;
using ll = long long;
static std::mt19937_64 rng((uint64_t)chrono::high_resolution_clock::now().time_since_epoch().count());

string timestamp() {
    auto now = chrono::system_clock::now();
    time_t t = chrono::system_clock::to_time_t(now);
    char buf[64];
    strftime(buf, sizeof(buf), "%F %T", localtime(&t));
    return string(buf);
}

int randint(int a,int b){ uniform_int_distribution<int> d(a,b); return d(rng); }
double randreal(double a,double b){ uniform_real_distribution<double> d(a,b); return d(rng); }
bool chance(double p){ return randreal(0.0,1.0) < p; }

vector<string> euclidParts = {"Rectangular Prayer Hall","Grand Dome","Minbar","Ablution Area","Courtyard","Portico","Minaret"};
vector<string> hyperbolicParts = {"Hyperbolic Arena","Saddle Cloister","Non-Euclidean Ablutions","Curved Minaret"};
vector<string> sphericalParts = {"Spherical Dome Cluster","Circulatory Courtyard","Ring Gallery","Panoramic Qibla Wall"};
vector<string> fractalParts = {"Fractal Chambers","Recursive Ablution Steps","Recursive Garden","Fractal Minaret"};
vector<string> ndParts = {"n-D Prayer Hyperplane","Tesseract Ablution Pod","Projective Mihrab","Dimensional Atrium"};

vector<string> amenitiesPool = {
    "Ablution Facilities", "Separate men's/women's prayer halls", "Wheelchair access & lifts",
    "Childcare & nursery", "Islamic library & study rooms", "Classrooms for Qur'an",
    "Community kitchen & food relief", "Primary healthcare clinic (basic)", "Counseling & social services",
    "Youth center & sports court", "Solar microgrid & battery", "Rainwater harvesting",
    "Digital learning lab (simulation)", "Emergency shelter capabilities", "Vocational workshops",
    "Small business incubator", "Green rooftop garden", "Public plaza & market stalls",
    "Translation & multi-language services", "Legal/mediation desk", "Cold storage for food aid",
    "Mobile outreach vehicle (simulation)"
};

string joinv(const vector<string>&v, const string &sep=", ") {
    string s;
    for(size_t i=0;i<v.size();++i){ s+=v[i]; if(i+1<v.size()) s+=sep; }
    return s;
}

struct Masjid {
    string id;
    string name;
    string mathSpace;
    vector<string> elements;
    vector<string> amenities;
    ll estimatedCostSim;
    atomic<bool> requestedFunding{false};
    atomic<bool> approved{false};
    atomic<bool> logged{false};
};

string randomName(){
    static vector<string> pref = {"Al-", "Noor ", "Salam ", "Ihsan ", "Rahma ", "Sakina ", "Barakah "};
    static vector<string> core = {"Jannah","Huda","Fajr","Mizan","Taqwa","Safa","Aman"};
    return pref[randint(0,pref.size()-1)] + core[randint(0,core.size()-1)];
}

// Generate a single masjid instance (no I/O)
Masjid generateMasjid(int serial){
    Masjid m;
    m.id = "MASJID-" + to_string(serial) + "-" + to_string(randint(1000,9999));
    m.name = randomName();
    int space = randint(0,4);
    if(space==0){ m.mathSpace="Euclidean (3D)"; m.elements=euclidParts; }
    else if(space==1){ m.mathSpace="Hyperbolic"; m.elements=hyperbolicParts; }
    else if(space==2){ m.mathSpace="Spherical"; m.elements=sphericalParts; }
    else if(space==3){ m.mathSpace="Fractal/Recursive"; m.elements=fractalParts; }
    else { m.mathSpace="Abstract n-D"; m.elements=ndParts; }
    if(chance(0.33)) m.elements.push_back("Fusion Atrium (multi-space junction)");
    // pick amenities
    vector<int> idx(amenitiesPool.size()); iota(idx.begin(), idx.end(), 0);
    shuffle(idx.begin(), idx.end(), rng);
    int k = randint(4, 10);
    for(int i=0;i<k && i<(int)idx.size(); ++i) m.amenities.push_back(amenitiesPool[idx[i]]);
    // ensure core items
    if(find(m.amenities.begin(), m.amenities.end(), "Ablution Facilities")==m.amenities.end())
        m.amenities.insert(m.amenities.begin(), "Ablution Facilities");
    if(find(m.amenities.begin(), m.amenities.end(), "Separate men's/women's prayer halls")==m.amenities.end())
        m.amenities.insert(m.amenities.begin()+1, "Separate men's/women's prayer halls");
    double base = 100000.0; // virtual base unit
    double complexity = 1.0 + m.elements.size()*0.15 + m.amenities.size()*0.09;
    m.estimatedCostSim = (ll)round(base * complexity * (0.6 + randreal(0.0,1.8)));
    return m;
}

// Thread-safe queue for generated masjids
class MQueue {
    mutex m;
    deque<shared_ptr<Masjid>> q;
public:
    void push(shared_ptr<Masjid> p){ lock_guard<mutex> lk(m); q.push_back(p); }
    shared_ptr<Masjid> pop() {
        lock_guard<mutex> lk(m);
        if(q.empty()) return nullptr;
        auto p = q.front(); q.pop_front(); return p;
    }
    vector<shared_ptr<Masjid>> snapshot() {
        lock_guard<mutex> lk(m);
        return vector<shared_ptr<Masjid>>(q.begin(), q.end());
    }
};

void printerThreadFunc(MQueue &mq, atomic<bool> &runFlag, atomic<ll> &simTreasury, atomic<ll> &simReserved) {
    while(runFlag) {
        auto batch = randint(1,3);
        for(int i=0;i<batch && runFlag;i++){
            auto p = mq.pop();
            if(!p){ this_thread::sleep_for(chrono::milliseconds(120)); continue; }
            cout << "\n=================================================================\n";
            cout << "[" << timestamp() << "] MASJID GENERATED: " << p->name << " (" << p->id << ")\n";
            cout << " MathSpace: " << p->mathSpace << "\n";
            cout << " Elements: " << joinv(p->elements) << "\n";
            cout << " Amenities: " << joinv(p->amenities) << "\n";
            cout << " EstimatedCost (SIM): " << p->estimatedCostSim << " virtual-units\n";
            cout << " Full-Stack Coverage Highlights:\n";
            cout << "  - Spiritual: prayer halls, study rooms, Mihrab/minbar provisions.\n";
            cout << "  - Social: kitchen, counseling, childcare.\n";
            cout << "  - Educational: classrooms, teacher incubator.\n";
            cout << "  - Health & Safety: triage room, emergency sheltering.\n";
            cout << "  - Infrastructure: solar microgrid estimate, water systems.\n";
            cout << "  - Economic: vocational workshops, micro-incubator.\n";
            cout << " Governance note: Qur'an-focused stewardship marker. Human approvals required for real-world actions.\n";
            cout << "=================================================================\n\n";
            // Randomly request funding (simulation) — requires human typed approval in main thread
            if(chance(0.12)) {
                p->requestedFunding = true;
                cout << "[ACTION] THIS MASJID IS REQUESTING SIMULATED FUNDING: To approve, switch to the console and follow the approval prompt.\n";
            }
            // small pause
            this_thread::sleep_for(chrono::milliseconds(randint(150,500)));
        }
        // occasional snapshot
        if(chance(0.25)) {
            cout << "[" << timestamp() << "] Snapshot: Simulated Treasury = " << simTreasury.load()
                 << "  Reserved = " << simReserved.load() << " virtual-units\n";
        }
        this_thread::sleep_for(chrono::milliseconds(randint(300,1200)));
    }
}

// Append approval record to a local log file (safe local storage)
void appendToLog(const string &entry) {
    try {
        ofstream f("masjid_approval_log.txt", ios::app);
        if(f) {
            f << entry << endl;
            f.close();
        } else {
            cerr << "[WARN] Could not open masjid_approval_log.txt for writing.\n";
        }
    } catch(...) {
        cerr << "[WARN] Failed to write to log file.\n";
    }
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cout << "AUTONOMOUS MASJID DESIGN STREAM (SIMULATION ONLY)\n";
    cout << "Generates continuous, richly-detailed masjid designs across multiple mathematical spaces.\n";
    cout << "IMPORTANT: ALL monetary values are SIMULATED. This program does NOT perform any real payments.\n";
    cout << "You are the human-in-the-middle. To mark simulated spending as 'approved', you must type a deliberate confirmation string.\n";
    cout << "Log file: masjid_approval_log.txt (local append-only log).\n\n";

    MQueue mq;
    atomic<bool> runFlag(true);
    atomic<ll> simTreasury(100000000); // large simulated treasury (virtual-units)
    atomic<ll> simReserved(0);
    thread printer;
    // generator thread: produces masjids and pushes to queue
    thread generator([&mq,&runFlag](){
        int serial = 1;
        while(runFlag){
            int burst = randint(1,4);
            for(int i=0;i<burst && runFlag;i++){
                auto m = make_shared<Masjid>(generateMasjid(serial++));
                mq.push(m);
                this_thread::sleep_for(chrono::milliseconds(randint(40,220)));
            }
            this_thread::sleep_for(chrono::milliseconds(randint(200,800)));
        }
    });
    // printer thread
    printer = thread(printerThreadFunc, ref(mq), ref(runFlag), ref(simTreasury), ref(simReserved));

    // Main thread: monitors queue and handles human approvals
    while(true){
        cout << "\nMAIN MENU: commands => [list] queued, [check] funding requests, [status], [approve <masjid-id>], [quit]\n> ";
        string cmd;
        if(!(cin >> cmd)) break;
        if(cmd=="quit" || cmd=="exit") {
            cout << "Shutting down...\n";
            runFlag = false;
            break;
        } else if(cmd=="list") {
            auto snap = mq.snapshot();
            cout << "Queued masjids: " << snap.size() << "\n";
            for(auto &p : snap) {
                cout << "  " << p->id << " | " << p->name << " | space: " << p->mathSpace
                     << " | estCost: " << p->estimatedCostSim
                     << " | requestedFunding: " << (p->requestedFunding ? "YES":"NO")
                     << " | approved: " << (p->approved ? "YES":"NO") << "\n";
            }
        } else if(cmd=="status") {
            cout << "SimulatedTreasury: " << simTreasury.load() << " virtual-units\n";
            cout << "SimulatedReserved: " << simReserved.load() << " virtual-units\n";
        } else if(cmd=="check") {
            // show any funding requests
            auto snap = mq.snapshot();
            int counter=0;
            for(auto &p : snap){
                if(p->requestedFunding && !p->approved){
                    cout << "[" << ++counter << "] Funding request -> " << p->id << " | " << p->name
                         << " | estCost: " << p->estimatedCostSim << " virtual-units\n";
                }
            }
            if(counter==0) cout << "No pending funding requests in queue.\n";
        } else if(cmd=="approve") {
            string masjidId;
            if(!(cin >> masjidId)) { cout << "Usage: approve <MASJID-ID>\n"; continue; }
            // find masjid in queue snapshot (note: generator may have moved it to printed)
            auto snap = mq.snapshot();
            shared_ptr<Masjid> target = nullptr;
            for(auto &p : snap){
                if(p->id == masjidId) { target = p; break; }
            }
            if(!target) {
                cout << "Masjid ID not found in queue snapshot. It may already have been printed. Approve from printed log by entering exact ID.\n";
                // Still allow approving (for safety) by creating a minimal approval record — but require typing full confirmation
            }
            // For safety: require two-step typed confirmations
            cout << "\nWARNING: This will only mark SIMULATED FUNDS AS 'APPROVED' IN THIS LOCAL SIMULATION LOG.\n";
            cout << "It does NOT transfer real funds. Real-world spending must be done separately under lawful governance.\n\n";
            cout << "Type the exact approval phrase to proceed:\n";
            cout << "  I APPROVE SPENDING SIMULATED " << " (type exactly)\n> ";
            string line;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            getline(cin, line);
            if(line != "I APPROVE SPENDING SIMULATED") {
                cout << "Approval phrase did not match. Aborting.\n";
                continue;
            }
            // Additional explicit confirmation typing the masjid id
            cout << "Type the MASJID ID to CONFIRM you authorize marking simulated funds as approved: ";
            string confirmId;
            getline(cin, confirmId);
            if(confirmId != masjidId) {
                cout << "Masjid ID mismatch. Aborting.\n";
                continue;
            }
            // Proceed: write an approval record to local log (append-only)
            string entry = timestamp() + " | APPROVAL (SIMULATION) | MASJID_ID=" + masjidId + " | approver=HUMAN-IN-MIDDLE"
                         + " | note='Approved simulated allocation of " + to_string(target ? target->estimatedCostSim : 0) + " virtual-units'"
                         + " | governance='Qur\\'an-focused stewardship (human oversight required)'.";
            appendToLog(entry);
            cout << "Recorded approval in local log: masjid_approval_log.txt\n";
            // adjust simulated reserved and mark target if present
            if(target){
                if(simTreasury.load() - simReserved.load() >= target->estimatedCostSim) {
                    simReserved += target->estimatedCostSim;
                    target->approved = true;
                    cout << "Simulated funds RESERVED for " << masjidId << " (virtual-units: " << target->estimatedCostSim << ").\n";
                } else {
                    cout << "[SIM-TREASURY] Insufficient simulated funds to reserve for this masjid. Approval logged but reservation failed.\n";
                }
            } else {
                cout << "Masjid not present in current queue snapshot. Approval logged for record-keeping; you must coordinate real-world treasury actions separately.\n";
            }
        } else {
            cout << "Unknown command. Valid commands: list, check, status, approve <MASJID-ID>, quit\n";
        }
    }

    // Join threads and exit
    runFlag = false;
    if(generator.joinable()) generator.join();
    if(printer.joinable()) printer.join();

    cout << "Exited. Final simulated treasury state: " << simTreasury.load()
         << " reserved: " << simReserved.load() << "\n";
    cout << "See masjid_approval_log.txt for recorded approvals.\n";
    return 0;
}
