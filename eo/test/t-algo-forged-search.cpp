
#include <iostream>
#include <string>
//#include <IOH.h>
#include <IOH_developing.h>
#include <eo>
#include <ga.h>
#include "../../problems/eval/oneMaxEval.h"
#include <cstdlib>

#include <eo>
#include <utils/checkpointing>

// using Particle = eoRealParticle<eoMaximizingFitness>;
using Int = eoInt<double, size_t>;
using Bits = eoBit<double, int>;



// Generate a search space of 5,232,000 algorithms,
// by enumerating candidate operators and their parameters.
eoAlgoFoundryEA<Bits>& make_foundry(eoFunctorStore& store, eoPopEvalFunc<Bits>& eval_onemax, int dimension, int pop_size)
{
    // doit être égal au eoInit plus bas sinon erreur
    auto& foundry = store.pack< eoAlgoFoundryEA<Bits> >(eval_onemax, dimension);

    // /***** Continuators ****/
    // for(size_t i=10; i < 30; i+=2 ) {
    //     foundry.continuators.add< eoSteadyFitContinue<Bits> >(10,i
    //     );
    // }

    /***** Continuators ****/
    foundry.continuators.add< eoGenContinue<Bits> >(2*dimension / pop_size);


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

int main(int argc, char* argv[])
{   
    
    // std::cout << "Checkpoint 1" << std::endl;

    eo::log << eo::setlevel(eo::warnings);
    eoFunctorStore store;

    // std::cout << "Checkpoint 2" << std::endl;

    //rng.seed() //eo:rng
    int dimension = 1000;
    // TO DO : linear on the y-axis ()
    IOHprofiler_ecdf_logger<int> logger(0, dimension, 100, 0, 2*dimension, 100);
    logger.set_complete_flag(true);
    logger.set_interval(0);

    logger.activate_logger();
    
    // std::cout << "Checkpoint 3" << std::endl;

    /// Configure w_model

    double w_model_suite_dummy_para = 0;
    int w_model_suite_epitasis_para = 0;
    int w_model_suite_neutrality_para = 0;
    int w_model_suite_ruggedness_para = 0;
    // std::cout << "Checkpoint 4" << std::endl;

    W_Model_OneMax w_model_om;
    std::string problem_name = "OneMax";
    problem_name = problem_name 
                    + "_D" + std::to_string((int)(w_model_suite_dummy_para * dimension))
                    + "_E" + std::to_string(w_model_suite_epitasis_para)
                    + "_N" + std::to_string(w_model_suite_neutrality_para)
                    + "_R" + std::to_string(w_model_suite_ruggedness_para);

    // std::cout << "Checkpoint 5" << std::endl;

    /// This must be called to configure the w-model to be tested.
    w_model_om.set_w_setting(w_model_suite_dummy_para,w_model_suite_epitasis_para,
                                    w_model_suite_neutrality_para,w_model_suite_ruggedness_para);

    // std::cout << "Checkpoint 6" << std::endl;

    /// Set problem_name based on the configuration.
    w_model_om.IOHprofiler_set_problem_name(problem_name);

    // std::cout << "Checkpoint 7" << std::endl;

    /// Set problem_id as 1
    w_model_om.IOHprofiler_set_problem_id(1);

    w_model_om.IOHprofiler_set_instance_id(atoi(argv[1]));

    

    // std::cout << "Checkpoint 8" << std::endl;

    /// Set dimension.
    w_model_om.IOHprofiler_set_number_of_variables(dimension);

    // std::cout << "Checkpoint 9" << std::endl;
    // std::cout << "Checkpoint 10" << std::endl;

    logger.track_problem(w_model_om);

    // std::cout << "Checkpoint 11" << std::endl;

    IOHprofiler_ecdf_sum sum;

    // std::cout << "Checkpoint 12" << std::endl;


    eoEvalIOHproblem<Bits> evalfunc(w_model_om, logger);
     
    eoPopLoopEval<Bits> onemax_eval(evalfunc);


    assert(argc == 8);
    // Int algo = {atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5])};
    Int algo(5);
    int pop_size = atoi(argv[3]);
    algo[0] = 0;
    algo[1] = atoi(argv[4]);
    algo[2] = atoi(argv[5]);
    algo[3] = atoi(argv[6]);
    algo[4] = atoi(argv[7]);

    auto& foundry = make_foundry(store, onemax_eval, dimension, pop_size);


    size_t n = foundry.continuators.size() * foundry.crossovers.size() * foundry.mutations.size() * foundry.selectors.size() * foundry.replacements.size();
    std::clog << n << " possible algorithms instances." << std::endl;

    // Evaluation of a forged algo on the sub-problem
    eoUniformGenerator<int> gen(0, 1);
    eoInitFixedLength<Bits> onemax_init(/*bitstring size=*/dimension, gen);
    eoEvalFoundryEA<Int, Bits> eval_foundry(foundry,
            onemax_init, /*pop_size=*/ pop_size,
            onemax_eval, /*penalization=*/ 0,
            sum, logger)   ;


    char *end;
    eo::rng.reseed(strtoul(argv[2], &end, 10));

    /***** return statistic on the chosen algorithm *****/


    eval_foundry(algo);
    


    // // Print a glimpse of the algorithm metric found.
    // //std::clog
    std::cout << "Algorithm's performance : " << std::endl;
    std::cout << algo.fitness() * -1 << std::endl;
}



