MODULE MainModule
! The module sends test signals to the ABB EGM interface in Simulink

	 VAR socketdev testsig_socket;
     VAR num torques{12};
     VAR rawbytes message;
     VAR intnum timeint; 
     PERS bool start_tsig:=TRUE;
    
    PROC main()
        TestSignReset;
        WaitUntil start_tsig=TRUE;
        FOR ii FROM 1 TO 6 DO
            TestSignDefine ii, 200+ii, ROB_1, 1,0; !Sygnaly z czujnika sily
           
        ENDFOR  
        PackRawBytes 0, message, 1 \Float4;
        SocketClose testsig_socket;
        SocketCreate testsig_socket \UDP;
                
        WHILE TRUE DO
            FOR ii FROM 1 TO 6 DO
                torques{ii} := TestSignRead(ii);
                PackRawBytes torques{ii}, message, 4*ii-3 \Float4;
            ENDFOR 
            SocketSendTo testsig_socket, "127.0.0.1", 6520, \RawData:=message;
            WaitTime 0.005;
        ENDWHILE
        
        
    ENDPROC
    
ENDMODULE