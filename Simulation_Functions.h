#pragma once

//main output function, Num_Diff_Agent_Pop is defined in log10 Range, NUm_Diff_Memory_Lengths is log2 range
void outputMinorityGameObservables(int NUM_STRATEGIES_PER_AGENT, int NUM_DAYS_AGENTS_PLAY, int NUM_DIFF_AGENT_POPS, int  NUM_DIFF_MEMORY_LENGTHS);

void output_Minority_Game_Attendance_History(int NUM_STRATEGIES_PER_AGENT, int NUM_DAYS_AGENTS_PLAY, unsigned int AGENT_POPULATION,
                                        int NUM_INDICES_IN_STRATEGY);

void alt_Strategy_Score_Update_Attendance_History(int NUM_STRATEGIES_PER_AGENT, int NUM_DAYS_AGENTS_PLAY, unsigned int AGENT_POPULATION,
                                             int NUM_INDICES_IN_STRATEGY);
void strategy_test_run(int NUM_STRATEGIES_PER_AGENT, int NUM_DAYS_AGENTS_PLAY, unsigned int AGENT_POPULATION,
                                             int NUM_INDICES_IN_STRATEGY);
