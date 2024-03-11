%%% function KV_Local_Param_Inp_Shv

%%%%% ===========================================
%%%  Disp_Debug_Yes = 0; %%% 0 1
%%%  
Disp_Debug_Yes = 1; %%% 0 1

%%%%% ===========================================
%%% 
KV_Input_Kinect = 1; %%% 1-Kinect; 0-Vicon
%%% KV_Input_Kinect = 0; %%% 1-Kinect; 0-Vicon

%%% OLD  Vicon_P1_In_YES = 1; %%% 1-P1_protcl; 0-P0_protcl(OPTIMIZ)
%%% OLD  Vicon_P1_In_YES = 0; %%% 1-P1_protcl; 0-P0_protcl(OPTIMIZ)

%%% 
Vicon_P1_In_YES =  [1 1 1 1 1]; %%% 1-P1_protcl; 0-P0_protcl(OPTIMIZ)  %%% P1_all: R_L_Legs(1 2) Trx(3) R_L_Hands(4 5) [1 1 1 1 1]-MAIN
%%% Vicon_P1_In_YES =  [0 0 0 0 0]; %%% 1-P1_protcl; 0-P0_protcl(OPTIMIZ)  %%% P0_all: R_L_Legs(1 2) Trx(3) R_L_Hands(4 5) [0 0 0 0 0]-All OPT
%%% Vicon_P1_In_YES =  [1 1 1 0 0]; %%% 1-P1_protcl; 0-P0_protcl(OPTIMIZ)  %%% P1_R_L_Legs(1 2) P1_Trx(3) P0_R_L_Hands(4 5) [1 1 1 0 0]-test
%%%%% ===========================================
%%% 
Reduced_Output_Kinect_Yes = 0; %%% 0-MAIN 1-analysis 'Opt_5_Bds'(see below)
%%% Reduced_Output_Kinect_Yes = 1; %%% 0-MAIN 1-analysis 'Opt_5_Bds'(see below)
%%%%% ===========================================
%%% Dir_Name_Data = 'Input_Kinect_Data\';
%%% Dir_Name_Data = 'Smth_Results\';
%%% 21may2012 SVSJ Kinect 
Dir_Name_Data = 'C:\ShV_2006\V_Sholukha\Work\System\Sh_Current_Wrk\Calibration\Suppl_Subr\Kinect_Microsoft_version0c\Test_CodeVictor_Serge\Kinect\';
%%% 21may2012 SVSJ Vicon Dir_Name_Data = 'C:\ShV_2006\V_Sholukha\Work\System\Sh_Current_Wrk\Calibration\Suppl_Subr\Kinect_Microsoft_version0c\Test_CodeVictor_Serge\Vicon\';
%%% 21may2012 SVSJ Vicon Dir_Name_Data = 'Test_CodeVictor_Serge\Vicon\';
%%% 21may2012 SVSJ Vicon 
Dir_Name_Data = 'Vicon\';
%%% 21may2012 SVSJ Kinect 
Dir_Name_Data = 'Kinect\';

%%%%% ===========================================
%%% Dir_Name_Results = 'Output_Kinect_Processed'; %%% 'test'  'test_06'  'test_07'
%%% Dir_Name_Results = 'test_13_G1R'; %%% 'test' 

%%% Dir_Name_Results = 'test_21M_S02_KV_UL'; %%%
%%% Dir_Name_Results = 'Session11_N1'; %%% 'test' 

%%% 
Dir_Name_Results = 'test_21M_KV_Walking_NewLabel'; %%%
%%% Dir_Name_Results = 'test_KV_WALK_BEST_1_N'; %%%
%%% Dir_Name_Results = 'KV_Squat_Kin_00'; %%%
%%% 
Dir_Name_Results = 'KV_Squat_Kin_S100'; %%%
%%% Dir_Name_Results = 'KV_Squat_P11100'; %%%
%%% Dir_Name_Results = 'KV_Squat_P11111'; %%%
%%% Dir_Name_Results = 'R_KV_Squat_P11111_S31'; %%%
%%% Dir_Name_Results = 'R_KV_Squat_P00001_T'; %%%
%%% Dir_Name_Results = 'KV_UL_N_ALs_P11111_T'; %%%
%%% Dir_Name_Results = 'KV_UL_N_ALs_P11111_10S'; %%%
%%% Dir_Name_Results = 'KV_UL_N_ALs_OPT'; %%%
%%%%% ===========================================
Static_Pose_Yes = 1; %%% 0 1-Scale from static pose
%%% FN_Kin_Data_Inp_Static = 'Static_Kinect_XYZ_20pnts_62cln.dat'; %%% e.g. Serge Squat 15may2012
%%% FN_Kin_Data_Inp_Static = 'Kinect_XYZ_ALs_R_Fem_Plv_002_S.dat'; %%% Serge Squat 15may2012
%%% FN_Kin_Data_Inp_Static = 'Kinect_XYZ_ALs_R_Fem_Plv_002_S0.dat'; %%% Serge Squat 15may2012
%%% FN_Kin_Data_Inp_Static = 'Kinect_XYZ_ALs_R_Fem_Plv_001.dat'; %%% Static Bruno 28 mar 2012
%%% FN_Kin_Data_Inp_Static = 'Kinect_XYZ_ALs_R_Fem_Plv_001_G.dat'; %%% Static Gerome 17apr2012

%%% 
FN_Kin_Data_Inp_Static = 'Static1'; %%% Serge Squat 21may2012 SVSJ
%%% FN_Kin_Data_Inp_Static = 'Static2'; %%% Serge Squat 21may2012 SVSJ
%%%%% ===========================================
%%% FN_Kin_Data_Inp = 'Motion_Kinect_XYZ_20pnts_62cln.dat'; %%% e.g. Serge Squat 15may2012
%%% FN_Kin_Data_Inp = 'Kinect_XYZ_ALs_R_Fem_Plv_002_S.dat'; %%% Serge Squat 15may2012
%%% FN_Kin_Data_Inp = 'Kinect_XYZ_ALs_R_Fem_Plv_002_S0.dat'; %%% Serge Squat 15may2012
%%% FN_Kin_Data_Inp = 'Kinect_XYZ_ALs_R_Fem_Plv_copy.dat'; %%% Squat Bruno 28 mar 2012
%%% FN_Kin_Data_Inp = 'Kinect_XYZ_ALs_R_Fem_Plv_007.dat'; %%% Walk Bruno 28 mar 2012
%%% FN_Kin_Data_Inp = 'Kinect_XYZ_ALs_R_Fem_Plv_006.dat'; %%% R_Lg_Abd Bruno 28 mar 2012
%%% FN_Kin_Data_Inp = 'Kinect_XYZ_ALs_R_Fem_Plv_005.dat'; %%% Squat Bruno 28 mar 2012

%%% FN_Kin_Data_Inp = 'Kinect_XYZ_ALs_R_Fem_Plv_006_G.dat'; %%% Gerome Squat 17apr2012
%%% FN_Kin_Data_Inp = 'Kinect_XYZ_ALs_R_Fem_Plv_007_G.dat'; %%% Gerome Walk 17apr2012
%%% FN_Kin_Data_Inp = 'Kinect_XYZ_ALs_R_Fem_Plv_011_G.dat'; %%% Gerome Upper 17apr2012 Upp LR_Abd
%%% FN_Kin_Data_Inp = 'Kinect_XYZ_ALs_R_Fem_Plv_011_G_1.dat'; %%% Gerome Upper 17apr2012 Upp LR_Abd
%%% FN_Kin_Data_Inp = 'Kinect_XYZ_ALs_R_Fem_Plv_012_G.dat'; %%% Gerome Upper 17apr2012 Upp LR_Flx
%%% FN_Kin_Data_Inp = 'Kinect_XYZ_ALs_R_Fem_Plv_012_G_1.dat'; %%% Gerome Upper 17apr2012 Upp LR_Flx
%%% FN_Kin_Data_Inp = 'Kinect_XYZ_ALs_R_Fem_Plv_013_G.dat'; %%% Gerome Upper 17apr2012 Upp LR_Flx
%%% FN_Kin_Data_Inp = 'Kinect_XYZ_ALs_R_Fem_Plv_013_G1.dat'; %%% Gerome Upper 17apr2012 Upp LR_Flx(100fr)
%%% FN_Kin_Data_Inp = 'Kinect_XYZ_ALs_R_Fem_Plv_016_G.dat'; %%% Gerome Passive 17apr2012
%%% FN_Kin_Data_Inp = 'Kinect_XYZ_ALs_R_Fem_Plv_005_G.dat'; %%% Gerome R_H_Kn Flx LL 17apr2012
%%% FN_Kin_Data_Inp = 'Kinect_XYZ_ALs_R_Fem_Plv_003_G.dat'; %%% Gerome R_H_Kn Abd LL 17apr2012

%%% FN_Kin_Data_Inp = 'Kinect_XYZ_ALs_R_Fem_Plv_001_S.dat'; %%% Serge Squat 15may2012
%%% FN_Kin_Data_Inp = 'Kinect_XYZ_ALs_R_Fem_Plv_002_S2.dat'; %%% Serge Squat 15may2012

%%% 
FN_Kin_Data_Inp = 'Session02'; %%% Serge Squat 21may2012 SVSJ
%%% FN_Kin_Data_Inp = 'TestUL2'; %%% Serge UpL Parall 21may2012 SVSJ
%%% FN_Kin_Data_Inp = 'Session11'; %%% Serge Walk 21may2012 SVSJ

%%% FN_Kin_Data_Inp = 'Squat.c3d'; %%% Serge Squat 21may2012 SVSJ
%%% FN_Kin_Data_Inp = 'Walking.c3d'; %%% Serge Walking 21may2012 SVSJ
%%% FN_Kin_Data_Inp = 'UL.c3d'; %%% Serge UL 21may2012 SVSJ

%%% FN_Kin_Data_Inp = 'Squat_NewLabel.c3d'; %%% Serge Squat 21may2012 SVSJ
%%% FN_Kin_Data_Inp = 'Walking_NewLabel.c3d'; %%% Serge Walking 21may2012 SVSJ
%%% FN_Kin_Data_Inp = 'WALK_BEST_1.c3d'; %%% Serge Walking 21may2012 SVSJ
%%% FN_Kin_Data_Inp = 'UL_N_ALs.c3d'; %%% Serge Walking 21may2012 SVSJ

%%%%% ===========================================
%%% 
Save_Model_Yes = 0; %%% 0-MAIN 1
%%% Save_Model_Yes = 1; %%% 0-MAIN 1
%%% 
Load_Model_Yes = 1; %%% 0-Requred ASCII:[TR72_3FN_MATL_LL.DAT, TR72_3FN_MATL_UL.DAT];  1-MAIN
%%% Load_Model_Yes = 0; %%% 0-Requred ASCII:[TR72_3FN_MATL_LL.DAT, TR72_3FN_MATL_UL.DAT];  1-MAIN
if Save_Model_Yes, Load_Model_Yes = 0; end
%%%%% ===========================================
%%% 
Opt_5_Bds = [1 1 1 1 1]; %%% R_L_Legs(1 2) Trx(3) R_L_Hands(4 5) [1 1 1 1 1]-MAIN
%%% Opt_5_Bds = [0 0 0 0 1]; %%% L_Hand(5) [0 0 0 0 1]-Test 
%%% Opt_5_Bds = [1 0 0 0 1]; %%% R_Leg L_Hand(1 5) [1 0 0 0 1]-Test 
%%% Opt_5_Bds = [1 0 0 0 0]; %%% R_Leg (1 ) [1 0 0 0 0]-Test 
%%% Opt_5_Bds = [0 1 0 0 0]; %%% L_Leg (2 ) [0 1 0 0 0]-Test 
%%% Opt_5_Bds = [1 1 0 0 0]; %%% RL_Leg (1 2) [1 1 0 0 0]-Test 
%%% Opt_5_Bds = [0 0 0 1 1]; %%% R_L_Hands(4 5) [0 0 0 1 1]-Test 
%%% Opt_5_Bds = [0 0 1 1 1]; %%% Trx(3) R_L_Hands(4 5) [0 0 1 1 1]-Test 
%%% Opt_5_Bds = [1 1 0 0 0]; %%% R_L_Legs(1 2) [1 1 0 0 0]-Test 
%%%%% ===========================================
%%% 
H_time_Refram = 0.1; %%% Normal Speed, e.g. SVSJ Squat_S02 21may2012
%%% H_time_Refram = 0.04; %%% Fast Speed, e.g. SVSJ Jump_S10 21may2012
%%% H_time_Refram = 0.01; %%% VERY Fast Speed, THEN(!!!) capturing frec(e.g.33fps) will be USED
%%%%% ===========================================
Equal_Frec_25_Yes = 1; %%% 0 1-MAIN; for Output reframing
%%% if Equal_Frec_25_Yes == 1, Frec_New = 100; h_Time = 1/Frec_New; end %%% 100 FPS
%%% if Equal_Frec_25_Yes == 1, Frec_New = 25; h_Time = 1/Frec_New; end %%% 25 FPS
%%% 
if Equal_Frec_25_Yes == 1, Frec_New = 10; h_Time = 1/Frec_New; end %%% 100 FPS
%%%%% ===========================================
%%% 
Thorax_Root_Yes = 0; %%% 1 0-MAIN
%%% Thorax_Root_Yes = 1; %%% 1 0-MAIN
if sum(Opt_5_Bds(1:3),2) == 0, Thorax_Root_Yes = 1; end 

%%%%% ===========================================
%%% 
Kinect_In_YES = 1; %%% 0 1-Kinect Data
%%% Kinect_In_YES = 0; %%% 0-Vicon Data 1
%%% Kinect_In_YES = KV_Input_Kinect;
IFLag_AL_Fout = 1; %%% 0 1-MAIN
LL_OPT_YES = 1; %%% 0 1-MAIN
UpL_OPT_YES = 1; %%% 0 1-MAIN
DoFs_Smooth_Yes = 1; %%% 0 1-MAIN
DoFs_Corr_Yes = 1; %%% 0 1-MAIN
Should_LR_Corr_Yes = 1; %%% 0 1-MAIN
Surfl_Yes = 0; %%% 1-Trx_Ellipsoid_Fig1, 0-MAIN
%%% Par_Smth_30 = 30; %%% 10-MAIN smoothing param for OVP spline
%%% 
Par_Smth_30 = 100; %%% 10-MAIN smoothing param for OVP spline
%%%%% ===========================================
x0_Poly_Apprx_Yes = [1 1 1 1 1]; %%% 1 0
%%% N_Polyn = [3 3 3 3 3]; %%% [2-5] polyn
%%% 
N_Polyn = [3 3 3 4 4]; %%% [2-5] polyn
%%% Mult_T5 = [0.5 0.5 0.5 0.0 0.0]; %%% [0.0-1.0]
%%% 
Mult_T5 = [0.5 0.5 0.5 0.3 0.3]; %%% [0.0-1.0]
%%% Mult_T5 = [0.5 0.5 0.5 0.5 0.5]; %%% [0.0-1.0]
%%%%% ===========================================
%%% 
Bord_Delt = 0.0001;
%%%%% ===========================================
%%% 
Start_Fr_N = 1; 
%%% Start_Fr_N = 263;
%%% Start_Fr_N = 201;
%%% Start_Fr_N = 301; %%% UP
%%% 
Decim_Frm = 4;
%%% Vic_Pelv_4ALs = [0 1 1 1]; %%%  !!! LIAS LIPS  RIAS RIPS 
%%% 
Vic_Pelv_4ALs = [1 1 1 1]; %%% MAIN  !!! LIAS LIPS  RIAS RIPS 
%%%%% ===========================================

%%% end %%% %%% function KV_Local_Param_Inp_Shv
