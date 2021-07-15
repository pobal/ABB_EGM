MODULE TestyEGM
!	Autor Pawel Obal
!	An example of the configuration program.
! 	EGM is in Position Guidance mode 

    VAR egmident egmID1;
    VAR egmstate egmSt1;
    VAR egmident egmID2;
    VAR egmstate egmSt2;
    CONST egm_minmax egm_minmax_lin1:=[-0.001,0.001];
    CONST egm_minmax egm_minmax_rot1:=[-0.01,0.01];
	TASK PERS wobjdata EGMTable:=[FALSE,TRUE,"",[[917.477,363.521,1251.49],[0.708156,-0.00430222,-0.000102162,-0.706043]],[[0,0,0],[1,0,0,0]]];
    CONST robtarget EGMPoint:=[[400,0,0],[0.00320405,-0.136795,0.990568,-0.00725065],[0,-1,-1,0],[9E+09,9E+09,9E+09,9E+09,9E+09,9E+09]];
	TASK PERS tooldata EGMSphere:=[TRUE,[[-0.0324819,-0.367353,260.971],[0.999806,0.00360442,0.00732421,0.0179062]],[2.24037,[2.32595,-1.06613,81.5204],[1,0,0,0],0,0,0]];
	TASK PERS loaddata EGMSphere_LD:=[2.24037,[2.32595,-1.06613,81.5204],[1,0,0,0],0,0,0];   
    
	PROC EGM_MoveTest()
        EGMStreamStop egmID1;
        EGMReset egmID1;
        EGMGetId egmID1;
        MoveL EGMPoint, v100, fine, EGMSphere;


        egmSt1:=EGMGetState(egmID1);
        TPWrite "EGM state: "\Num:=egmSt1;
        egmSt2:=EGMGetState(egmID2);
        TPWrite "EGM state: "\Num:=egmSt2;
        
        IF egmSt1 <= EGM_STATE_CONNECTED THEN
            EGMSetupUC ROB_1, egmID1, "Move", "UCdevice" \Joint \CommTimeout:=30 ;
        ENDIF
        
       EGMActPose egmID1 \StreamStart \Tool:=EGMSphere \Wobj:=EGMTable2, corr_frame_offs, EGM_FRAME_WOBJ,  EGMSphere.tframe, EGM_FRAME_TOOL
       \x:=egm_minmax_lin1 \Y:=egm_minmax_lin1 
        \Z:=egm_minmax_lin1 \Rx:=egm_minmax_rot1
        \Ry:=egm_minmax_rot1 \Rz:=egm_minmax_rot1 
        \SampleRate:=8
        \MaxPosDeviation:=50  \MaxSpeedDeviation:=50 ;
      
        EGMRunPose egmID1, EGM_STOP_RAMP_DOWN \x \y \z \CondTime:=10 \RampInTime:=0.001 \RampOutTime:=0.001 \PosCorrGain:=0;
        egmSt1:=EGMGetState(egmID1);
        IF egmSt1 = EGM_STATE_CONNECTED THEN
            TPWrite "Reset EGM instance egmID1";
            EGMStreamStop egmID1;
            EGMReset egmID1;
        ENDIF
        
        TPWrite "EGM state: "\Num:=egmSt1;
        IF egmSt1 <= EGM_STATE_RUNNING THEN
            TPWrite "EGM state: Running";
        ELSE
             TPWrite "EGM state: "\Num:=egmSt1;
        ENDIF
        
        EGMReset egmID1;
                
    ENDPROC
        

    
    
   

ENDMODULE