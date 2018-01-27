#include "Evolutionary_Minority_Game.h"
#include <random>

enum {
    AGENT_POPULATION = 301,
    NUM_INDICIES_IN_STRATEGY = 7,
    //RUN_TIME = 10000+NUM_INDICIES_IN_STRATEGY,
    RUN_TIME = 1000,
    NUM_STRATEGIES_PER_AGENT = 2,
    RNG_SEED = 42,
    NUM_DIFF_POPULATIONS = 8, //101 to 701
    NUM_DIFF_MEMORY_LENGTHS = 16 , //2 to 16
    AGENTS_IDENTIFIER = 0,
    NUM_DIFF_STRATEGY_SETS = 10,
    EVOLUTION_MEMORY = 25,
};

class RandomAgent : public Agent {
    int seed;
    std::mt19937 generator;
    std::uniform_int_distribution<int> dist;
public:
    RandomAgent (int seed) : seed {seed}, generator {(unsigned int)seed}, dist {0, 1} {}

    virtual signum get_prediction(const MarketHistory &history) {
        return dist(generator) ? 1 : -1;
    }

    virtual void update(const MarketHistory &history, signum market_result) {};
    virtual void print() {
        printf ("RandomAgent seed %i\n", seed);
    }
};

AgentPool alpha_agents() {
    AgentPool agents;
    for (int i = 0; i < AGENT_POPULATION; ++i) {
        agents.push_back (std::unique_ptr<Agent> {new AlphaAgent{i, NUM_STRATEGIES_PER_AGENT, NUM_INDICIES_IN_STRATEGY}});
    }
    return std::move (agents);
}
AgentPool random_agents() {
    AgentPool agents;
    for (int i = 0; i < AGENT_POPULATION; ++i) {
        agents.push_back (std::unique_ptr<Agent> {new RandomAgent{i}});
    }
    return std::move (agents);
}

class BasicEvolution : public EvolutionStrategy {
    int last_used_agent_id;
public:
    BasicEvolution (int initial_agent_population) : last_used_agent_id (initial_agent_population - 1) {}

    virtual std::vector<Agent*> select_next_generation (const MarketHistory& history, AgentPool& agent_pool) override {
        std::vector<Agent*> next_gen = history.last_day().agents();

        next_gen.pop_back();

        next_gen.insert (next_gen.begin(), create_agent (agent_pool, new RandomAgent {last_used_agent_id + 1}));
        ++last_used_agent_id;

        return std::move (next_gen);
    }
};


int convert (double x) {
    return (int) x;
}
int foo (std::function<int(double)> f) {
    return f (2.0);
}

foo (convert);

using namespace std;
std::vector<MarketDay> basic_pre_history(int size, int seed, int num_agents) {
    auto binary_pre_history = random_signum_vector (size, seed);
    auto market_pre_history = market_history_generator (binary_pre_history, num_agents, seed);
    std::vector<MarketDay> result;

    for (int i  = 0; i != size; ++i) {
        result.emplace_back (i - size, market_pre_history[i], binary_pre_history[i]);
    }
    return std::move (result);
}



int main() {
    ExperimentState experiment {basic_pre_history(NUM_INDICIES_IN_STRATEGY, RNG_SEED, AGENT_POPULATION), std::unique_ptr<EvolutionStrategy> {new BasicEvolution {AGENT_POPULATION}}, random_agents()};
    experiment.simulate (RUN_TIME);
    experiment.print();
    /*

    Experiment environment{AGENT_POPULATION, NUM_STRATEGIES_PER_AGENT,
                           NUM_INDICIES_IN_STRATEGY, AGENTS_IDENTIFIER,
                           EVOLUTION_MEMORY, RNG_SEED};
    environment.run_minority_game(RUN_TIME);
    for (auto& a : environment.agents) {
        a.print();
    }
    environment.write_attendance_history();
    environment.write_memory_distribution();
    environment.write_minority_game_observables(RUN_TIME, NUM_DIFF_POPULATIONS,
                                                NUM_DIFF_MEMORY_LENGTHS, NUM_STRATEGIES_PER_AGENT,
                                                EVOLUTION_MEMORY, NUM_DIFF_STRATEGY_SETS);
                                                */
    return 0;
}