#include <iostream>
#include <string>
#include <IOH.h>
#include <eo>
#include <ga.h>
#include "../../problems/eval/oneMaxEval.h"

#include <eo>
#include <utils/checkpointing>

using Particle = eoRealParticle<eoMaximizingFitness>;
using Bits = eoBit<double, int>;



// Generate a search space of 5,232,000 algorithms,
// by enumerating candidate operators and their parameters.
eoAlgoFoundryEA<Bits>& make_foundry(eoFunctorStore& store, eoPopEvalFunc<Bits>& eval_onemax)
{
    auto& foundry = store.pack< eoAlgoFoundryEA<Bits> >(eval_onemax, 100);

    /***** Continuators ****/
    for(size_t i=10; i < 30; i+=2 ) {
        foundry.continuators.add< eoSteadyFitContinue<Bits> >(10,i);
    }

    /***** Crossovers ****/
    foundry.crossovers.add< eo1PtBitXover<Bits> >();
    for(double i=0.1; i<0.9; i+=0.1) {
        foundry.crossovers.add< eoUBitXover<Bits> >(i); // preference over 1
    }
    for(size_t i=1; i < 11; i+=1) {
        foundry.crossovers.add< eoNPtsBitXover<Bits> >(i); // nb of points
    }

    /***** Mutations ****/
    for(double i=0.01; i<1.0; i+=0.01) { 
        foundry.mutations.add< eoBitMutation<Bits> >(i); // proba of flipping any bit
    }
    for(size_t i=1; i < 11; i+=1) {
        foundry.mutations.add< eoDetBitFlip<Bits> >(i); // mutate k bits
    }

    /***** Selectors *****/
    for(double i=0.51; i<0.91; i+=0.1) {
        foundry.selectors.add< eoStochTournamentSelect<Bits> >(i);
    }
    foundry.selectors.add< eoSequentialSelect<Bits> >();
    foundry.selectors.add< eoProportionalSelect<Bits> >();
    for(size_t i=2; i < 10; i+=1) {
        foundry.selectors.add< eoDetTournamentSelect<Bits> >(i);
    }

    /***** Replacements ****/
    foundry.replacements.add< eoCommaReplacement<Bits> >();
    foundry.replacements.add< eoPlusReplacement<Bits> >();
    foundry.replacements.add< eoSSGAWorseReplacement<Bits> >();
    for(double i=0.51; i<0.91; i+=0.1) {
        foundry.replacements.add< eoSSGAStochTournamentReplacement<Bits> >(i);
    }
    for(size_t i=2; i < 10; i+=1) {
        foundry.replacements.add< eoSSGADetTournamentReplacement<Bits> >(i);
    }

    return foundry;
}

// A basic PSO algorithm.
std::pair< eoAlgo<Particle>*, eoPop<Particle>* >
    make_pso(eoFunctorStore& store, eoEvalFoundryEA<Particle,Bits>& eval_foundry, size_t dim)
{
    auto& gen_pos = store.pack< eoUniformGenerator<double> >(0.1,0.9);
    auto& random_pos = store.pack< eoInitFixedLength<Particle> >(dim, gen_pos);

    auto pop = new eoPop<Particle>();
    pop->append(10, random_pos); // pop size

    auto& gen_minus = store.pack< eoUniformGenerator<double> >(-0.05, 0.05);
    auto& random_velo = store.pack< eoVelocityInitFixedLength<Particle> >(dim, gen_minus);

    auto& local_init = store.pack< eoFirstIsBestInit<Particle> >();

    auto topology = new eoLinearTopology<Particle>(5); // neighborhood size

    auto& init = store.pack< eoInitializer<Particle> >(eval_foundry, random_velo, local_init, *topology, *pop);
    init();

    auto bounds = new eoRealVectorBounds(dim, 0, 0.999999);

    auto& velocity = store.pack< eoStandardVelocity<Particle> >(*topology, 1, 1.6, 2, *bounds);

    auto& flight = store.pack< eoStandardFlight<Particle> >();

    auto& cont_gen = store.pack< eoGenContinue<Particle> >(10);
    auto& cont = store.pack< eoCombinedContinue<Particle> >(cont_gen);

    auto& checkpoint = store.pack< eoCheckPoint<Particle> >(cont);
    auto& best = store.pack< eoBestFitnessStat<Particle> >();
    checkpoint.add(best);
    auto& monitor = store.pack< eoOStreamMonitor >(std::clog);
    monitor.add(best);
    checkpoint.add(monitor);

    auto& pso = store.pack< eoEasyPSO<Particle> >(init, checkpoint, eval_foundry, velocity, flight);

    return std::make_pair(&pso,pop);
}    


int main(int /*argc*/, char** /*argv*/)
{   
    
    eo::log << eo::setlevel(eo::warnings);
    eoFunctorStore store;



    // IOHprofiler_csv_logger<int> logger("./","run_w_model","EA","EA");
    // logger.set_complete_flag(true);
    // logger.set_interval(0);

    // logger.activate_logger();
    
    // /// Configure w_model
    // int dimension = 100;
    // double w_model_suite_dummy_para = 0.75;
    // int w_model_suite_epitasis_para = 4;
    // int w_model_suite_neutrality_para = 3;
    // int w_model_suite_ruggedness_para = 100;

    // W_Model_OneMax w_model_om;
    // std::string problem_name = "OneMax";
    // problem_name = problem_name 
    //                 + "_D" + std::to_string((int)(w_model_suite_dummy_para * dimension))
    //                 + "_E" + std::to_string(w_model_suite_epitasis_para)
    //                 + "_N" + std::to_string(w_model_suite_neutrality_para)
    //                 + "_R" + std::to_string(w_model_suite_ruggedness_para);

    // /// This must be called to configure the w-model to be tested.
    // w_model_om.set_w_setting(w_model_suite_dummy_para,w_model_suite_epitasis_para,
    //                                 w_model_suite_neutrality_para,w_model_suite_ruggedness_para);

    // /// Set problem_name based on the configuration.
    // w_model_om.IOHprofiler_set_problem_name(problem_name);

    // /// Set problem_id as 1
    // w_model_om.IOHprofiler_set_problem_id(1);

    // /// Set dimension.
    // w_model_om.IOHprofiler_set_number_of_variables(dimension);

    // logger.track_problem(w_model_om.IOHprofiler_get_problem_id(), 
    //                     w_model_om.IOHprofiler_get_number_of_variables(), 
    //                     w_model_om.IOHprofiler_get_instance_id(),
    //                     w_model_om.IOHprofiler_get_problem_name(),
    //                     w_model_om.IOHprofiler_get_optimization_type());




    IOHprofiler_ecdf_logger<int> logger(0,4e7,0, 0,100,20);
    logger.set_complete_flag(true);
    logger.set_interval(0);

    logger.activate_logger();
    
    /// Configure w_model
    int dimension = 100;
    double w_model_suite_dummy_para = 0;
    int w_model_suite_epitasis_para = 0;
    int w_model_suite_neutrality_para = 0;
    int w_model_suite_ruggedness_para = 0;

    W_Model_OneMax w_model_om;
    std::string problem_name = "OneMax";
    problem_name = problem_name 
                    + "_D" + std::to_string((int)(w_model_suite_dummy_para * dimension))
                    + "_E" + std::to_string(w_model_suite_epitasis_para)
                    + "_N" + std::to_string(w_model_suite_neutrality_para)
                    + "_R" + std::to_string(w_model_suite_ruggedness_para);

    /// This must be called to configure the w-model to be tested.
    w_model_om.set_w_setting(w_model_suite_dummy_para,w_model_suite_epitasis_para,
                                    w_model_suite_neutrality_para,w_model_suite_ruggedness_para);

    /// Set problem_name based on the configuration.
    w_model_om.IOHprofiler_set_problem_name(problem_name);

    /// Set problem_id as 1
    w_model_om.IOHprofiler_set_problem_id(1);

    /// Set dimension.
    w_model_om.IOHprofiler_set_number_of_variables(dimension);

    logger.track_problem(w_model_om.IOHprofiler_get_problem_id(), 
                        w_model_om.IOHprofiler_get_number_of_variables(), 
                        w_model_om.IOHprofiler_get_instance_id(),
                        w_model_om.IOHprofiler_get_problem_name(),
                        w_model_om.IOHprofiler_get_optimization_type());


    
    eoEvalIOHproblem<Bits> evalfunc(w_model_om, logger);
    eoPopLoopEval<Bits> onemax_eval(evalfunc);

    auto& foundry = make_foundry(store, onemax_eval);

    size_t n = foundry.continuators.size() * foundry.crossovers.size() * foundry.mutations.size() * foundry.selectors.size() * foundry.replacements.size();
    std::clog << n << " possible algorithms instances." << std::endl;

    // Evaluation of a forged algo on the sub-problem
    eoBooleanGenerator gen(0.5);
    eoInitFixedLength<Bits> onemax_init(/*bitstring size=*/50, gen);
    eoEvalFoundryEA<Particle,Bits> eval_foundry(foundry,
            onemax_init, /*pop_size=*/ 10,
            onemax_eval, /*penalization=*/ 0)   ;


    /***** Algorithm selection stuff (irace on foundry) *****/






    











    /***** Algorithm selection stuff (PSO on foundry) *****/
    eoAlgo<Particle>* pso;
    eoPop<Particle>* pop_foundry;
    std::tie(pso, pop_foundry) = make_pso(store, eval_foundry, foundry.size());
 
    // Perform the best algorithm configuration search.
    (*pso)(*pop_foundry);











    // // Print a glimpse of the best algorithm found.
    foundry.select(eval_foundry.decode(pop_foundry->best_element()));
    std::cout << "Best algorithm: " << foundry.name() << std::endl;

}