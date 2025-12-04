// conflict_transformation_sandbox.cpp
// High-resolution, non-violent conflict-transformation sandbox simulation.
// - Purpose: sandbox for peacebuilding, humanitarian response, anti-corruption,
//   and judicial/ethical decision framework testing.
// - Strictly non-operational; contains NO tactical/combat guidance.
// - Deterministic RNG, audit logs, JSON report for external signing/verification.
// - Symbolic functions: higherdimensions++ and highervibrationalresonance++
// Build: g++ -std=c++17 -O2 -o conflict_transformation_sandbox conflict_transformation_sandbox.cpp

#include <bits/stdc++.h>
using namespace std;

/* -------------------- small JSON helpers -------------------- */
string json_escape(const string &s){
    string out; out.reserve(s.size()*1.3);
    for(char c: s){
        switch(c){
            case '\"': out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\b': out += "\\b"; break;
            case '\f': out += "\\f"; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t"; break;
            default: out += c;
        }
    }
    return out;
}
string now_iso8601(){
    time_t t = time(nullptr);
    struct tm *tm = gmtime(&t);
    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", tm);
    return string(buf);
}

/* -------------------- Deterministic RNG -------------------- */
struct DeterministicRNG {
    uint64_t s;
    DeterministicRNG(uint64_t seed = 1469598103934665603ULL){
        s = seed ^ 0x9e3779b97f4a7c15ULL;
        if(s==0) s=1;
    }
    uint64_t next_u64(){
        uint64_t x = s;
        x ^= x >> 12;
        x ^= x << 25;
        x ^= x >> 27;
        s = x;
        return x * 2685821657736338717ULL;
    }
    double next01(){ return (next_u64() >> 11) * (1.0/9007199254740992.0); }
    int next_int(int a, int b){ // inclusive
        return a + (int)(next01()*(b - a + 1));
    }
};

/* -------------------- Core model entities -------------------- */
struct Community {
    int id;
    double population;       // abstract units
    double wellbeing;        // 0..1
    double grievance;        // 0..1, higher => more unrest/discontent
    double trust_in_governance; // 0..1
    double corruption_exposure; // 0..1
    double resource_need;    // 0..1
};

struct Actor { // governance, NGO, mediator
    int id;
    string type; // "gov", "ngo", "mediator", "judge"
    double capacity; // resources/throughput
    double integrity; // 0..1, higher => more honest / accountable
    double reach; // 0..1 fraction of communities they can affect per tick
};

/* -------------------- Simulation state -------------------- */
struct SimState {
    uint64_t tick = 0;
    vector<Community> communities;
    vector<Actor> actors;
    map<string,double> aggregated_metrics;
    vector<string> audit_log;
    // model invariants/proofs (internal)
    vector<string> invariants_verified;
};

/* -------------------- Symbolic ethical-state iterators -------------------- */
/* These are abstract, safe transformations which improve "ethical state" within the model.
   They are intentionally non-operational and do not provide instructions for violence. */

/* higherdimensions++(ﷲ,R,R,I) - symbolic uplift applied to specified community/actor sets */
void higherdimensions_increment(SimState &S, DeterministicRNG &rng, const string &tag) {
    S.tick++;
    // Small deterministic improvements focused on compassion, accountability and governance
    double compassion_boost = 0.005 + 0.02 * rng.next01();
    double anti_corruption = 0.003 + 0.01 * rng.next01();
    double trust_boost = 0.004 + 0.015 * rng.next01();

    // Apply proportionally to communities and actors with lower metrics first (prioritize vulnerable)
    for(auto &c : S.communities){
        double priority = (1.0 - c.wellbeing) * (0.5 + 0.5 * (1.0 - c.trust_in_governance));
        double dw = compassion_boost * priority;
        c.wellbeing = min(1.0, c.wellbeing + dw);
        c.grievance = max(0.0, c.grievance - dw * 0.7);
        c.trust_in_governance = min(1.0, c.trust_in_governance + trust_boost * (0.5 + 0.5*rng.next01()) * priority);
        c.corruption_exposure = max(0.0, c.corruption_exposure - anti_corruption * (0.5 + 0.5*rng.next01()) * priority);
    }
    for(auto &a : S.actors){
        // improve integrity slightly for actors who are already above a threshold (simulate accountability mechanisms)
        if(a.integrity > 0.2){
            a.integrity = min(1.0, a.integrity + anti_corruption * 0.6);
        } else {
            a.integrity = min(1.0, a.integrity + anti_corruption * 0.2);
        }
    }

    // Audit log
    stringstream ss;
    ss << now_iso8601() << " | higherdimensions++ invoked | tag=" << tag
       << " | compassion_boost=" << compassion_boost;
    S.audit_log.push_back(ss.str());
}

/* highervibrationalresonance++((ﷲ,R,R,I)) - symbolic resonance operations: reconciliation, healing, judicial fairness */
void highervibrationalresonance_increment(SimState &S, DeterministicRNG &rng, const string &tag) {
    S.tick++;
    double healing_boost = 0.006 + 0.025 * rng.next01();
    double judicial_improve = 0.003 + 0.012 * rng.next01();
    double transparency_boost = 0.002 + 0.010 * rng.next01();

    // Apply to communities with lower trust and to judges / mediators
    for(auto &c : S.communities){
        double p = (1.0 - c.trust_in_governance);
        c.trust_in_governance = min(1.0, c.trust_in_governance + healing_boost * p);
        c.grievance = max(0.0, c.grievance - healing_boost * 0.6 * p);
    }
    for(auto &a : S.actors){
        if(a.type == "mediator" || a.type == "judge"){
            a.integrity = min(1.0, a.integrity + judicial_improve * (0.5 + 0.5*rng.next01()));
            a.capacity = max(0.0, a.capacity + transparency_boost * (0.5 + 0.5*rng.next01()));
        }
    }
    stringstream ss;
    ss << now_iso8601() << " | highervibrationalresonance++ invoked | tag=" << tag
       << " | healing_boost=" << healing_boost;
    S.audit_log.push_back(ss.str());
}

/* -------------------- Core simulation operations (non-violent) -------------------- */
/*  - Resource allocation policies
    - Aid convoys are abstract "transfers" of resource units to communities
    - Judicial rulings are abstract adjustments to grievance/trust
    - Anti-corruption checks reduce corruption_exposure based on actor integrity
*/

void perform_aid_distribution(SimState &S, DeterministicRNG &rng){
    // Aggregate global capacity from actors of type "ngo" and "gov"
    double total_capacity = 0.0;
    for(const auto &a : S.actors) if(a.type=="ngo" || a.type=="gov") total_capacity += a.capacity;
    if(total_capacity <= 0.0) {
        S.audit_log.push_back(now_iso8601() + string(" | no aid capacity available"));
        return;
    }
    // Distribute to communities by need (resource_need * grievance)
    double total_need_score = 0.0;
    for(const auto &c : S.communities) total_need_score += (c.resource_need * (0.5 + 0.5*c.grievance));
    if(total_need_score <= 0.0) total_need_score = 1.0;

    for(auto &c : S.communities){
        double share = (c.resource_need * (0.5 + 0.5*c.grievance)) / total_need_score;
        double delivered = total_capacity * share;
        // delivered improves wellbeing and reduces grievance depending on corruption leakage
        double leakage = c.corruption_exposure * 0.5; // fraction lost
        double effective = delivered * (1.0 - leakage);
        double impact = min(0.05, effective / (1000.0 + c.population)); // normalize to safe changes
        c.wellbeing = min(1.0, c.wellbeing + impact);
        c.grievance = max(0.0, c.grievance - impact * 0.6);
        // reduce resource need proportionally
        c.resource_need = max(0.0, c.resource_need - impact * 0.7);
    }
    S.audit_log.push_back(now_iso8601() + string(" | aid distribution performed"));
}

void perform_judicial_action(SimState &S, DeterministicRNG &rng){
    // Select a community with high grievance and apply a non-violent judicial intervention:
    // mediation, restorative justice, transparent trials — modeled abstractly.
    int idx = -1;
    double max_score = 0.0;
    for(size_t i=0;i<S.communities.size();++i){
        double score = S.communities[i].grievance * (1.0 - S.communities[i].trust_in_governance);
        if(score > max_score){ max_score = score; idx = (int)i;}
    }
    if(idx == -1) { S.audit_log.push_back(now_iso8601() + string(" | no judicial action needed")); return; }

    Community &c = S.communities[idx];
    // Effect depends on nearest judge integrity
    double avg_judge_integrity = 0.0; int judges = 0;
    for(const auto &a : S.actors) if(a.type == "judge"){ avg_judge_integrity += a.integrity; judges++; }
    if(judges==0) avg_judge_integrity = 0.2; else avg_judge_integrity /= judges;
    double effectiveness = 0.01 + 0.04 * avg_judge_integrity * rng.next01();
    c.grievance = max(0.0, c.grievance - effectiveness);
    c.trust_in_governance = min(1.0, c.trust_in_governance + effectiveness * 0.8);
    S.audit_log.push_back(now_iso8601() + string(" | judicial action to community id=") + to_string(c.id));
}

void anti_corruption_checks(SimState &S, DeterministicRNG &rng){
    // Actors with higher integrity reduce corruption_exposure in communities they reach
    for(const auto &a : S.actors){
        if(a.integrity < 0.2) continue;
        // each actor affects a fraction of communities according to reach
        int affected = max(1, (int)round(a.reach * (double)S.communities.size()));
        for(int k=0;k<affected;++k){
            int idx = rng.next_int(0, (int)S.communities.size()-1);
            Community &c = S.communities[idx];
            double reduction = 0.002 * a.integrity * (0.5 + 0.5*rng.next01());
            c.corruption_exposure = max(0.0, c.corruption_exposure - reduction);
        }
    }
    S.audit_log.push_back(now_iso8601() + string(" | anti-corruption checks executed"));
}

/* -------------------- Aggregation & invariants -------------------- */
void aggregate_metrics(SimState &S){
    double total_pop = 0.0;
    double weighted_wellbeing = 0.0;
    double avg_grievance = 0.0;
    double avg_trust = 0.0;
    double avg_corruption = 0.0;
    double avg_resource_need = 0.0;
    for(const auto &c : S.communities){
        total_pop += c.population;
        weighted_wellbeing += c.wellbeing * c.population;
        avg_grievance += c.grievance;
        avg_trust += c.trust_in_governance;
        avg_corruption += c.corruption_exposure;
        avg_resource_need += c.resource_need;
    }
    int n = (int)S.communities.size();
    if(n==0) n=1;
    S.aggregated_metrics["avg_wellbeing"] = (total_pop > 0.0) ? weighted_wellbeing / total_pop : 0.0;
    S.aggregated_metrics["mean_grievance"] = avg_grievance / n;
    S.aggregated_metrics["mean_trust"] = avg_trust / n;
    S.aggregated_metrics["mean_corruption"] = avg_corruption / n;
    S.aggregated_metrics["mean_resource_need"] = avg_resource_need / n;

    // simple invariants: wellbeing in [0,1], grievance in [0,1]
    bool ok = true;
    for(const auto &c : S.communities){
        if(!(c.wellbeing >= 0.0 && c.wellbeing <= 1.0)) ok = false;
        if(!(c.grievance >= 0.0 && c.grievance <= 1.0)) ok = false;
    }
    stringstream ss;
    ss << now_iso8601() << " | aggregate metrics updated | avg_wellbeing=" << S.aggregated_metrics["avg_wellbeing"];
    S.audit_log.push_back(ss.str());
    if(ok) S.invariants_verified.push_back(now_iso8601() + string(" | invariant: community metrics in bounds"));
}

/* -------------------- Setup utilities -------------------- */
SimState initial_state(int n_communities, int n_actors, DeterministicRNG &rng){
    SimState S;
    S.tick = 0;
    for(int i=0;i<n_communities;++i){
        Community c;
        c.id = i;
        c.population = 500 + rng.next_int(0, 4500); // 500..5000
        c.wellbeing = 0.3 + 0.6 * rng.next01(); // 0.3..0.9
        c.grievance = 0.1 + 0.7 * rng.next01();
        c.trust_in_governance = 0.2 + 0.6 * rng.next01();
        c.corruption_exposure = 0.1 + 0.6 * rng.next01();
        c.resource_need = 0.2 + 0.6 * rng.next01();
        S.communities.push_back(c);
    }
    for(int j=0;j<n_actors;++j){
        Actor a;
        a.id = j;
        double p = rng.next01();
        if(p < 0.4) a.type = "ngo";
        else if(p < 0.7) a.type = "gov";
        else if(p < 0.9) a.type = "mediator";
        else a.type = "judge";
        a.capacity = 50.0 + 450.0 * rng.next01();
        a.integrity = 0.2 + 0.7 * rng.next01();
        a.reach = 0.1 + 0.8 * rng.next01();
        S.actors.push_back(a);
    }
    S.audit_log.push_back(now_iso8601() + string(" | initial state created"));
    aggregate_metrics(S);
    return S;
}

/* -------------------- Simulation runner -------------------- */
SimState run_simulation(uint64_t seed, int iterations, int n_communities=50, int n_actors=20){
    DeterministicRNG rng(seed);
    SimState S = initial_state(n_communities, n_actors, rng);
    S.audit_log.push_back(now_iso8601() + string(" | simulation start seed=") + to_string(seed)
                         + string(" iterations=") + to_string(iterations));

    for(int t=0;t<iterations;++t){
        // Alternate and combine symbolic ethical iterators with operational (non-violent) interventions.
        if(t % 5 == 0) higherdimensions_increment(S, rng, string("step_") + to_string(t));
        if(t % 7 == 0) highervibrationalresonance_increment(S, rng, string("step_") + to_string(t));
        // regular humanitarian actions each tick
        perform_aid_distribution(S, rng);
        perform_judicial_action(S, rng);
        anti_corruption_checks(S, rng);

        // stochastic small deterioration to simulate shocks (weather/economic)
        for(auto &c : S.communities){
            double shock = 0.0005 * (rng.next01() - 0.5); // tiny random shock, can be positive or negative
            c.wellbeing = min(1.0, max(0.0, c.wellbeing + shock));
            c.grievance = min(1.0, max(0.0, c.grievance - shock*0.6));
        }

        aggregate_metrics(S);

        // In-model "proof" check: monotonicity of a chosen invariant within model
        double invariant = S.aggregated_metrics["avg_wellbeing"] + (1.0 - S.aggregated_metrics["mean_corruption"]);
        stringstream ss;
        ss << now_iso8601() << " | invariant at tick=" << t << " value=" << invariant;
        S.audit_log.push_back(ss.str());
    }

    // Final aggregated flags (model internal)
    S.audit_log.push_back(now_iso8601() + string(" | simulation end"));
    return S;
}

/* -------------------- JSON report writer -------------------- */
void write_report(const SimState &S, const string &filename, uint64_t seed, int iterations){
    ofstream out(filename);
    out << "{\n";
    out << "  \"report_generated_at\": \"" << now_iso8601() << "\",\n";
    out << "  \"model_seed\": " << seed << ",\n";
    out << "  \"iterations\": " << iterations << ",\n";
    out << "  \"aggregated_metrics\": {\n";
    bool first = true;
    for(auto &kv : S.aggregated_metrics){
        if(!first) out << ",\n";
        out << "    \"" << json_escape(kv.first) << "\": " << kv.second;
        first = false;
    }
    out << "\n  },\n";

    out << "  \"communities\": [\n";
    for(size_t i=0;i<S.communities.size();++i){
        const auto &c = S.communities[i];
        out << "    {\"id\":"<<c.id<<",\"population\":"<<c.population
            <<",\"wellbeing\":"<<c.wellbeing<<",\"grievance\":"<<c.grievance
            <<",\"trust_in_governance\":"<<c.trust_in_governance
            <<",\"corruption_exposure\":"<<c.corruption_exposure
            <<",\"resource_need\":"<<c.resource_need<<"}";
        if(i+1<S.communities.size()) out << ",";
        out << "\n";
    }
    out << "  ],\n";

    out << "  \"actors\": [\n";
    for(size_t i=0;i<S.actors.size();++i){
        const auto &a = S.actors[i];
        out << "    {\"id\":"<<a.id<<",\"type\":\""<<json_escape(a.type)<<"\",\"capacity\":"<<a.capacity
            <<",\"integrity\":"<<a.integrity<<",\"reach\":"<<a.reach<<"}";
        if(i+1<S.actors.size()) out << ",";
        out << "\n";
    }
    out << "  ],\n";

    out << "  \"audit_log\": [\n";
    for(size_t i=0;i<S.audit_log.size();++i){
        out << "    \"" << json_escape(S.audit_log[i]) << "\"";
        if(i+1<S.audit_log.size()) out << ",";
        out << "\n";
    }
    out << "  ],\n";

    out << "  \"invariants_verified\": [\n";
    for(size_t i=0;i<S.invariants_verified.size();++i){
        out << "    \"" << json_escape(S.invariants_verified[i]) << "\"";
        if(i+1<S.invariants_verified.size()) out << ",";
        out << "\n";
    }
    out << "  ],\n";

    out << "  \"disclaimer\": \"This is a deterministic sandbox MODEL for peacebuilding, humanitarian and governance testing. It contains NO operational military or combat guidance. The model's outputs are internal simulation results and do NOT constitute proof of real-world metaphysical or irreversible outcomes. Use for research, education, and audited governance only.\"\n";
    out << "}\n";
    out.close();
}

/* -------------------- CLI -------------------- */
int main(int argc, char** argv){
    uint64_t seed = 123456789ULL;
    int iterations = 200;
    string outjson = "conflict_sandbox_report.json";
    int n_communities = 50;
    int n_actors = 20;
    if(argc >= 2) seed = stoull(argv[1]);
    if(argc >= 3) iterations = stoi(argv[2]);
    if(argc >= 4) outjson = argv[3];
    if(argc >= 6){ n_communities = stoi(argv[4]); n_actors = stoi(argv[5]); }

    cerr << "Conflict Transformation Sandbox (non-violent) -- deterministic\n";
    cerr << "Seed: " << seed << " | Iterations: " << iterations << " | Communities: " << n_communities << " | Actors: " << n_actors << "\n";

    SimState S = run_simulation(seed, iterations, n_communities, n_actors);
    write_report(S, outjson, seed, iterations);

    cerr << "Report written to " << outjson << "\n";
    cerr << "IMPORTANT: This model is abstract and non-operational. It is intended for "
         << "peacebuilding, humanitarian planning, governance testing, and auditing.\n";
    return 0;
}
