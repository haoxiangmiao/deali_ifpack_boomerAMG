
#include <BoomerAMG_solver.h>

DEAL_II_NAMESPACE_OPEN

namespace TrilinosWrappers
{

BoomerAMG_Parameters::BoomerAMG_Parameters(){
	/*
	parameters.insert( {"interp_type", parameter_data(100, & HYPRE_BoomerAMGSetInterpType)} );
	parameters.insert( {"coarsen_type", parameter_data(6, & HYPRE_BoomerAMGSetCoarsenType)} );
	parameters.insert( {"relax_type", parameter_data(0, & HYPRE_BoomerAMGSetRelaxType)} );
	parameters.insert( {"max_hypre_itter", parameter_data(50, & HYPRE_BoomerAMGSetMaxIter)} );
	parameters.insert( {"max_amg_levels", parameter_data(40, & HYPRE_BoomerAMGSetMaxLevels)} );
	parameters.insert( {"amg_cycle_type", parameter_data(6, & HYPRE_BoomerAMGSetCycleType)} );
	parameters.insert( {"sabs_flag", parameter_data(6, & HYPRE_BoomerAMGSetSabs)} );

	parameters.insert( {"distance_R", parameter_data(2.0, & HYPRE_BoomerAMGSetRestriction)} );
	parameters.insert( {"strength_tolC", parameter_data(2.0, & HYPRE_BoomerAMGSetStrongThreshold)} );
	parameters.insert( {"strength_tolR", parameter_data(2.0, & HYPRE_BoomerAMGSetStrongThresholdR)} );
	parameters.insert( {"filterA_tol", parameter_data(2.0, & HYPRE_BoomerAMGSetADropTol)} );
	parameters.insert( {"post_filter_R", parameter_data(2.0, & HYPRE_BoomerAMGSetFilterThresholdR)} );
	*/

	parameters.insert( {"hypre_print_level", parameter_data(100, & HYPRE_BoomerAMGSetPrintLevel)} );
	parameters.insert( {"coarsen_type", parameter_data(6, & HYPRE_BoomerAMGSetCoarsenType)} );
	parameters.insert( {"relax_type", parameter_data(0, & HYPRE_BoomerAMGSetRelaxType)} );
	parameters.insert( {"number_sweeps", parameter_data(1, & HYPRE_BoomerAMGSetNumSweeps)} );
	parameters.insert( {"max_itter", parameter_data(50, & HYPRE_BoomerAMGSetMaxIter)} );

	parameters.insert( {"solve_tol", parameter_data(1e-10, & HYPRE_BoomerAMGSetTol)} );


}

void BoomerAMG_Parameters::set_parameters(Ifpack_Hypre & Ifpack_obj, const Hypre_Chooser solver_preconditioner_selection){

	apply_parameter_variant_visitor parameter_visitor(Ifpack_obj, solver_preconditioner_selection);

	for (auto param_itter=parameters.begin();param_itter!=parameters.end();++param_itter){
		if ((param_itter->second).set_function == nullptr){
			boost::apply_visitor(parameter_visitor, (param_itter->second).hypre_function, (param_itter->second).value );
		} else{
			(param_itter->second).set_function(solver_preconditioner_selection, param_itter->second , Ifpack_obj);
		}
	}
}

void BoomerAMG_Parameters::set_relaxation_order(const Hypre_Chooser solver_preconditioner_selection, const parameter_data & param_data, Ifpack_Hypre & Ifpack_obj){

	std::pair<std::string,std::string> param_value = boost::get< std::pair<std::string,std::string> >(param_data.value);

	const unsigned int ns_down = param_value.first.length();
	const unsigned int ns_up = param_value.second.length();
	const unsigned int ns_coarse = 1 ;

	const std::string F("F");
	const std::string C("C");
	const std::string A("A");

	// Array to store relaxation scheme and pass to Hypre
	int **grid_relax_points = (int **) malloc(4*sizeof(int *));
	grid_relax_points[0] = NULL;
	grid_relax_points[1] = (int *) malloc(sizeof(int)*ns_down);
	grid_relax_points[2] = (int *) malloc(sizeof(int)*ns_up);
	grid_relax_points[3] = (int *) malloc(sizeof(int));
	grid_relax_points[3][0] = 0;

	// set down relax scheme
	for(unsigned int i = 0; i<ns_down; i++) {
	    if (param_value.first.compare(i,1,F) == 0) {
	       grid_relax_points[1][i] = -1;
	    }
	    else if (param_value.first.compare(i,1,C) == 0) {
	       grid_relax_points[1][i] = 1;
	    }
	    else if (param_value.first.compare(i,1,A) == 0) {
	       grid_relax_points[1][i] = 0;
	    }
	 }

	 // set up relax scheme
	 for(unsigned int i = 0; i<ns_up; i++) {
	    if (param_value.second.compare(i,1,F) == 0) {
	       grid_relax_points[2][i] = -1;
	    }
	    else if (param_value.second.compare(i,1,C) == 0) {
	       grid_relax_points[2][i] = 1;
	    }
	    else if (param_value.second.compare(i,1,A) == 0) {
	       grid_relax_points[2][i] = 0;
	    }
	 }

	//Ifpack_obj.SetParameter(solver_preconditioner_selection , & HYPRE_BoomerAMGSetGridRelaxPoints , ns_coarse,3);
	Ifpack_obj.SetParameter(solver_preconditioner_selection , & HYPRE_BoomerAMGSetCycleNumSweeps , ns_coarse,3);
	Ifpack_obj.SetParameter(solver_preconditioner_selection , & HYPRE_BoomerAMGSetCycleNumSweeps , ns_down,1);
	Ifpack_obj.SetParameter(solver_preconditioner_selection , & HYPRE_BoomerAMGSetCycleNumSweeps , ns_up,2);

}

}
DEAL_II_NAMESPACE_CLOSE
