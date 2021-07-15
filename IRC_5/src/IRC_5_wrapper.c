
/*
 * Include Files
 *
 */
#include "simstruc.h"



/* %%%-SFUNWIZ_wrapper_includes_Changes_BEGIN --- EDIT HERE TO _END */
#include <math.h>
    #include <iostream>
    #include <string>
    #include <vector>
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <winerror.h>
    #include "egm.pb.h"
    #pragma comment(lib, "Ws2_32.lib")      // socket lib
    #pragma comment(lib, "Mswsock.lib") 
    #pragma comment(lib, "AdvApi32.lib") 
    #pragma comment(lib, "libprotobuf.lib")

    using namespace std;
    using namespace abb::egm;
/* %%%-SFUNWIZ_wrapper_includes_Changes_END --- EDIT HERE TO _BEGIN */
#define u_width 1
#define y_width 1

/*
 * Create external references here.  
 *
 */
/* %%%-SFUNWIZ_wrapper_externs_Changes_BEGIN --- EDIT HERE TO _END */
/* extern double func(double a); */
    static unsigned int sequenceNumber = 0;
    struct sockaddr_in serverAddr, clientAddr;
    struct sockaddr_in tsigserveAddr, tsigclientAddr;
    SOCKET sockfd;
    SOCKET sockstr;
    static int err = 0;
    static char msgOut[256];
    static char msgErr[256];
/* %%%-SFUNWIZ_wrapper_externs_Changes_END --- EDIT HERE TO _BEGIN */

/*
 * Start function
 *
 */
void IRC_5_Start_wrapper(void **pW,
			const real_T *port_1, const int_T p_width0,
			const real_T *port_2, const int_T p_width1,
			const real_T *orient_switch, const int_T p_width2,
			const uint32_T *tout, const int_T p_width3,
			SimStruct *S)
{
/* %%%-SFUNWIZ_wrapper_Start_Changes_BEGIN --- EDIT HERE TO _END */
ssSetOptions(S, SS_OPTION_CALL_TERMINATE_ON_EXIT);
    int portNumber_1;
	int portNumber_2;
    memset(msgOut, 0, 256);
    memset(msgErr, 0, 256);
    int tout_fd =  static_cast<int>(*tout);
    int tout_str =  2;
	int bufSize_fd = 500;      // EGM socket buffer size
	int bufSize_srt = 48;    // TestSignals buffer size
    int bufSndSize_fd = 1000;      // EGM send socket buffer size
    portNumber_1 = static_cast<int>(*port_1);
	portNumber_2 = static_cast<int>(*port_2);
    // EGM socket option
    int get_tout_fd;
	int optlen_fd = sizeof(get_tout_fd);
     int get_sndtout_fd;
	int sndoptlen_fd = sizeof(get_tout_fd);
	int get_tout_str;
	int optlen_str = sizeof(get_tout_str);
    // TestSignals socket option
	int get_buff_fd;
	int optlen_buff_fd = sizeof(get_buff_fd);
    int get_sndbuff_fd;
	int optlen_sndbuff_fd = sizeof(get_sndbuff_fd);
	int get_buff_str;
	int optlen_buff_str = sizeof(get_buff_str);
    fd_set read_fds;
    struct timeval timeout;
    timeout.tv_sec = 1;
	timeout.tv_usec = 0;
    
    EgmRobot* pRobotMessage = new EgmRobot();
	pW[0] = pRobotMessage;
	EgmSensor* pSensorMessage = new EgmSensor();
	pW[1] = pSensorMessage;
    
    WSADATA wsaData;
	err = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (err != NO_ERROR)
	{
		sprintf(msgErr, "WSAStartup failed with error: %u\n", WSAGetLastError());
		ssSetErrorStatus(S, msgErr);
		WSACleanup();

	}
    //EGM Socket
    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    memset(&serverAddr, 0,sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(portNumber_1);    
    //TestSignals Socket
    sockstr = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    memset(&tsigserveAddr, 0, sizeof(tsigserveAddr));
    tsigserveAddr.sin_family = AF_INET;
	tsigserveAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	tsigserveAddr.sin_port = htons(portNumber_2);
    //ssPrintf("Socket II O.K.\n");
    //Binding
    err = bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if (err == SOCKET_ERROR)
	{
		sprintf(msgErr, "EGM bind failed with error: %u\n", WSAGetLastError());
		ssSetErrorStatus(S, msgErr);
		WSACleanup();
		return;
	}
    err = bind(sockstr, (struct sockaddr*)&tsigserveAddr, sizeof(tsigserveAddr));
	if (err == SOCKET_ERROR)
	{

		sprintf(msgErr, "TestSignal bind failed with error: %u\n", WSAGetLastError());
		ssSetErrorStatus(S, msgErr);
		WSACleanup();
		return;
	}
    //ssPrintf("Bind II O.K.\n");
    // Select
    FD_ZERO(&read_fds);
    FD_SET(sockfd, &read_fds);
    int smax = max(sockstr, sockfd);
	err = select(smax + 1, &read_fds, NULL, NULL, &timeout);
	if (err == SOCKET_ERROR)
	{
		sprintf(msgErr, "Select is failed with error: %u\n", WSAGetLastError());
		ssSetErrorStatus(S, msgErr);
		return;
	}
    //Setting paramter
    err = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&tout_fd, sizeof(tout_fd));
	if (err == SOCKET_ERROR)
	{
		sprintf(msgErr, "EGM Setsocket timeout is failed with error: %u\n", WSAGetLastError());
		ssSetErrorStatus(S, msgErr);
		return;
	}
    err = setsockopt(sockstr, SOL_SOCKET, SO_RCVTIMEO, (char*)&tout_str, sizeof(tout_str));
	if (err == SOCKET_ERROR)
	{
		sprintf(msgErr, "TestSignal Setsocket timeout is failed with error: %u\n", WSAGetLastError());
		ssSetErrorStatus(S, msgErr);
		return;
	}
    ssPrintf("SET timeout O.K.\n");
    err = setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (char*)&bufSize_fd, sizeof(bufSize_fd));
	if (err == SOCKET_ERROR)
	{
		sprintf(msgErr, "EGM Setsocket buffer is failed with error: %u\n", WSAGetLastError());
		ssSetErrorStatus(S, msgErr);
		return;
	}
    err = setsockopt(sockstr, SOL_SOCKET, SO_RCVBUF, (char*)&bufSize_srt, sizeof(bufSize_srt));
	if (err == SOCKET_ERROR)
	{
		sprintf(msgErr, "TestSignal Setsocket buffer is failed with error: %u\n", WSAGetLastError());
		ssSetErrorStatus(S, msgErr);
		return;
	}
    //Set send buffer 
//     err = setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (char*)&bufSndSize_fd, sizeof(bufSndSize_fd));
// 	if (err == SOCKET_ERROR)
// 	{
// 		sprintf(msgErr, "EGM Setsocket send buffer is failed with error: %u\n", WSAGetLastError());
// 		ssSetErrorStatus(S, msgErr);
// 		return;
// 	}
//      err = setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (char*)&tout_fd, sizeof(tout_fd));
// 	if (err == SOCKET_ERROR)
// 	{
// 		sprintf(msgErr, "EGM Setsocket send timeout is failed with error: %u\n", WSAGetLastError());
// 		ssSetErrorStatus(S, msgErr);
// 		return;
// 	}
    ssPrintf("SET recvbuf O.K.\n");
    err = getsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&get_tout_fd, &optlen_fd);
	if (err == SOCKET_ERROR)
	{
		sprintf(msgErr, "EGM Getsocket timeout failed with error: %u\n", WSAGetLastError());
		ssSetErrorStatus(S, msgErr);
		return;
	}
    else
    {
        sprintf(msgOut, "EGM Recvtimeout size: %u ms\n", get_tout_fd);
        ssPrintf(msgOut);
    }
    err = getsockopt(sockstr, SOL_SOCKET, SO_RCVTIMEO, (char*)&get_tout_str, &optlen_str);
	if (err == SOCKET_ERROR)
	{
		sprintf(msgErr, "TestSignal Getsocket timeout failed with error: %u\n", WSAGetLastError());
		ssSetErrorStatus(S, msgErr);
		return;
	}
    else
    {
        sprintf(msgOut, "TestSignal Recvtimeout size: %u ms\n", get_tout_str);
        ssPrintf(msgOut);
    }
    err = getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (char*)&get_buff_fd, &optlen_buff_fd);
	if (err == SOCKET_ERROR)
	{
		sprintf(msgErr, "EGM Getsocket buffer option failed with error: %u\n", WSAGetLastError());
		ssSetErrorStatus(S, msgErr);
		return;
	}
    else
    {   sprintf(msgOut, "EGM Recvbuffer size: %u\n", get_buff_fd);
        ssPrintf(msgOut);
    }
    err = getsockopt(sockstr, SOL_SOCKET, SO_RCVBUF, (char*)&get_buff_str, &optlen_buff_str);
	if (err == SOCKET_ERROR)
	{
		sprintf(msgErr, "TestSignal Getsocket buffer option failed with error: %u\n", WSAGetLastError());
		ssSetErrorStatus(S, msgErr);
		return;
	}
    else
    {   sprintf(msgOut, "TestSignal Recvbuffer size: %u\n", get_buff_str);
        ssPrintf(msgOut);
    }
    err = getsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (char*)&get_sndbuff_fd, &optlen_sndbuff_fd);
	if (err == SOCKET_ERROR)
	{
		sprintf(msgErr, "EGM Getsocket send buffer option failed with error: %u\n", WSAGetLastError());
		ssSetErrorStatus(S, msgErr);
		return;
	}
    else
    {   sprintf(msgOut, "EGM Sendbuffer size size: %u\n", get_sndbuff_fd);
        ssPrintf(msgOut);
    }
    err = getsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (char*)&get_sndtout_fd, &sndoptlen_fd);
	if (err == SOCKET_ERROR)
	{
		sprintf(msgErr, "EGM Getsocket send buffer option failed with error: %u\n", WSAGetLastError());
		ssSetErrorStatus(S, msgErr);
		return;
	}
    else
    {   sprintf(msgOut, "EGM Sendtimeout size size: %u\n", get_sndtout_fd);
        ssPrintf(msgOut);
    }
    // Check connection
    if (FD_ISSET(sockfd, &read_fds))
	{
		ssPrintf("EGM socket connected.\n");
	}
	else
	{
		sprintf(msgErr, "EGM socket not connected. Error code: %u\n", WSAGetLastError());
		ssSetErrorStatus(S, msgErr);
		
	}
    FD_SET(sockstr, &read_fds);
	if (FD_ISSET(sockstr, &read_fds))
	{
		ssPrintf("TestSignal socket connected.\n");
	}
	else
	{
		sprintf(msgErr, "TestSignal socket not connected. Error code: %u\n", WSAGetLastError());
		ssSetErrorStatus(S, msgErr);
	}
    //ssPrintf("Start END\n");
/* %%%-SFUNWIZ_wrapper_Start_Changes_END --- EDIT HERE TO _BEGIN */
}
/*
 * Output function
 *
 */
void IRC_5_Outputs_wrapper(const real_T *triger,
			const real_T *position,
			const real_T *euler_ang,
			const real_T *quaternions,
			const real_T *joints,
			const real_T *j_speed,
			const real_T *cart_speed,
			real_T *curr_position,
			real_T *curr_angle,
			real_T *curr_quat,
			real_T *curr_joints,
			real_T *force,
			real_T *IRC5_time,
			real_T *testsig,
			void **pW,
			const real_T *port_1, const int_T p_width0,
			const real_T *port_2, const int_T p_width1,
			const real_T *orient_switch, const int_T p_width2,
			const uint32_T *tout, const int_T p_width3,
			SimStruct *S)
{
/* %%%-SFUNWIZ_wrapper_Outputs_Changes_BEGIN --- EDIT HERE TO _END */
char tsigMessage[48];
    memset(tsigMessage, 0, 48);
    char protoMessage[500];
    memset(protoMessage, 0, 500);
    string messageBuffer;
    int flag = 0;
    float testsig_value=0;
    int len_1 = sizeof(clientAddr);
    int len_2 = sizeof(tsigclientAddr);
    int iResult;
    int eResult;
    int sResult;
    time_t time_IRC5;
	struct tm* timeinfo;
	double tm_usec;
     
    EgmRobot* pRobotMessage = static_cast<EgmRobot*>(pW[0]);
	EgmSensor* pSensorMessage = static_cast<EgmSensor*>(pW[1]);
    
    //Receive EGM Robot Message
    eResult = recvfrom(sockfd, protoMessage, 1400, flag, (struct sockaddr*)&clientAddr, &len_1);
	if (eResult == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAETIMEDOUT)
		{
			//sprintf(msgErr, "EM response time exceeded, error: %u\n", WSAGetLastError());
			//ssSetErrorStatus(S, msgErr);
			//return;
		}
		else
		{
			sprintf(msgErr, "Recvfrom failed with error: %u\n", WSAGetLastError());
			ssSetErrorStatus(S, msgErr);
			return;
		}
	}
    else
    {
        pRobotMessage->ParseFromArray(protoMessage, eResult);
    	if (pRobotMessage->has_feedback())
    	{
       		curr_position[0] = pRobotMessage->feedback().cartesian().pos().x();
    		curr_position[1] = pRobotMessage->feedback().cartesian().pos().y();
    		curr_position[2] = pRobotMessage->feedback().cartesian().pos().z();
    		curr_angle[0] = pRobotMessage->feedback().cartesian().euler().x();
    		curr_angle[1] = pRobotMessage->feedback().cartesian().euler().y();
    		curr_angle[2] = pRobotMessage->feedback().cartesian().euler().z();
    		curr_quat[0] = pRobotMessage->feedback().cartesian().orient().u0();
    		curr_quat[1] = pRobotMessage->feedback().cartesian().orient().u1();
    		curr_quat[2] = pRobotMessage->feedback().cartesian().orient().u2();
    		curr_quat[3] = pRobotMessage->feedback().cartesian().orient().u3();
    		time_IRC5 = pRobotMessage->feedback().time().sec();
    		timeinfo = _localtime64(&time_IRC5);
    		tm_usec = pRobotMessage->feedback().time().usec();
    		IRC5_time[0] = timeinfo->tm_hour;
    		IRC5_time[1] = timeinfo->tm_min;
    		IRC5_time[2] = timeinfo->tm_sec + (tm_usec / 1000000);
    		IRC5_time[3] = tm_usec;
    		for (int n = 0; n < pRobotMessage->measuredforce().force_size(); n++)
    		{
    			force[n] = pRobotMessage->measuredforce().force(n);
    		}
    
    		for (int n = 0; n < pRobotMessage->feedback().joints().joints_size(); n++)
    		{
    			curr_joints[n] = pRobotMessage->feedback().joints().joints(n);
    		}
		
	    }
    }
     // Create EGMSensor message
    //Header
    EgmHeader* header = new EgmHeader();
	header->set_mtype(EgmHeader_MessageType_MSGTYPE_CORRECTION);
	header->set_seqno(sequenceNumber++);
	header->set_tm(GetTickCount());
    pSensorMessage->set_allocated_header(header);
    // TCP Position
	EgmCartesian* pc = new EgmCartesian();
	pc->set_x(position[0]);
	pc->set_y(position[1]);
	pc->set_z(position[2]);
    // TCP orientarion in euler angle
	EgmEuler* pe = new EgmEuler;
	pe->set_x(euler_ang[0]);
	pe->set_y(euler_ang[1]);
	pe->set_z(euler_ang[2]);
    // TCP orientation in quaternions
    EgmQuaternion* pq = new EgmQuaternion();
	pq->set_u0(quaternions[0]);
	pq->set_u1(quaternions[1]);
	pq->set_u2(quaternions[2]);
	pq->set_u3(quaternions[3]);
	EgmPose* pcartesian = new EgmPose();
    pcartesian->set_allocated_pos(pc);
    if(orient_switch) pcartesian->set_allocated_orient(pq);
    else  pcartesian->set_allocated_euler(pe);
    EgmJoints* jk = new EgmJoints;
	jk->add_joints(joints[0]);
	jk->add_joints(joints[1]);
	jk->add_joints(joints[2]);
	jk->add_joints(joints[3]);
	jk->add_joints(joints[4]);
	jk->add_joints(joints[5]);
    EgmPlanned* planned = new EgmPlanned();
    planned->set_allocated_joints(jk);
    planned->set_allocated_cartesian(pcartesian);
   	EgmCartesianSpeed* cs = new EgmCartesianSpeed();
	cs->add_value(cart_speed[0]);
    cs->add_value(cart_speed[1]);
    cs->add_value(cart_speed[2]);
    cs->add_value(cart_speed[3]);
    cs->add_value(cart_speed[4]);
    cs->add_value(cart_speed[5]);
    EgmJoints* js = new EgmJoints;
	js->add_joints(j_speed[0]);
    js->add_joints(j_speed[1]);
    js->add_joints(j_speed[2]);
    js->add_joints(j_speed[3]);
    js->add_joints(j_speed[4]);
    js->add_joints(j_speed[5]);
    EgmSpeedRef* sr = new EgmSpeedRef;
	sr->set_allocated_joints(js);
	sr->set_allocated_cartesians(cs);
    pSensorMessage->set_allocated_planned(planned);
    pSensorMessage->set_allocated_speedref(sr);
    pSensorMessage->SerializeToString(&messageBuffer);
    sResult = sendto(sockfd, messageBuffer.c_str(), messageBuffer.length(), 0, (struct sockaddr*)&clientAddr, sizeof(clientAddr));
	if (sResult <= 0)
	{
		sprintf(msgOut, "Error send message: %u\n", WSAGetLastError());
		ssSetErrorStatus(S, msgOut);
		return;
	}
    
    //Receive TestSignals
    iResult = recvfrom(sockstr, tsigMessage, 48,flag, (struct sockaddr*)&tsigserveAddr, &len_2);
	if (iResult == SOCKET_ERROR)
	{            
		if (WSAGetLastError() == WSAETIMEDOUT)
		{
			//sprintf(msgErr, "TestSignal response time exceeded, error: %u\n", WSAGetLastError());
          	//ssSetErrorStatus(S, msgErr);
            //return;
		}
		else
		{   
            sprintf(msgErr, "Recvfrom failed II with error: %u\n",  WSAGetLastError()); 
            ssSetErrorStatus(S, msgErr);
            return;
		}
    
    }
    else
    {
        memcpy(&testsig_value, tsigMessage + iResult -48, sizeof(float));
    	testsig[0] = testsig_value;
    	memcpy(&testsig_value, tsigMessage + iResult - 48 +4, sizeof(float));
    	testsig[1] = testsig_value;
    	memcpy(&testsig_value, tsigMessage + iResult - 48 +8, sizeof(float));
    	testsig[2] = testsig_value;
    	memcpy(&testsig_value, tsigMessage + iResult - 48 +12, sizeof(float));
    	testsig[3] = testsig_value;
    	memcpy(&testsig_value, tsigMessage + iResult - 48 +16, sizeof(float));
    	testsig[4] = testsig_value;
    	memcpy(&testsig_value, tsigMessage + iResult - 48 +20, sizeof(float));
        testsig[5] = testsig_value;
        memcpy(&testsig_value, tsigMessage + iResult -48 +24, sizeof(float));
    	testsig[6] = testsig_value;
    	memcpy(&testsig_value, tsigMessage + iResult - 48 +28, sizeof(float));
    	testsig[7] = testsig_value;
    	memcpy(&testsig_value, tsigMessage + iResult - 48 +32, sizeof(float));
    	testsig[8] = testsig_value;
    	memcpy(&testsig_value, tsigMessage + iResult - 48 +36, sizeof(float));
    	testsig[9] = testsig_value;
    	memcpy(&testsig_value, tsigMessage + iResult - 48 +40, sizeof(float));
    	testsig[10] = testsig_value;
    	memcpy(&testsig_value, tsigMessage + iResult - 48 +44, sizeof(float));
        testsig[11] = testsig_value;
	}
/* %%%-SFUNWIZ_wrapper_Outputs_Changes_END --- EDIT HERE TO _BEGIN */
}

/*
 * Terminate function
 *
 */
void IRC_5_Terminate_wrapper(void **pW,
			const real_T *port_1, const int_T p_width0,
			const real_T *port_2, const int_T p_width1,
			const real_T *orient_switch, const int_T p_width2,
			const uint32_T *tout, const int_T p_width3,
			SimStruct *S)
{
/* %%%-SFUNWIZ_wrapper_Terminate_Changes_BEGIN --- EDIT HERE TO _END */
EgmRobot* pRobotMessage = static_cast<EgmRobot*>(pW[0]);
	EgmSensor* pSensorMessage = static_cast<EgmSensor*>(pW[1]);
	delete pRobotMessage;
	delete pSensorMessage;
    err = closesocket(sockfd);
	if (err == SOCKET_ERROR)
	{
		sprintf(msgErr, "Close EGM socket failed with error: %u\n", WSAGetLastError());
		ssSetErrorStatus(S, msgErr);
        
	}
    err = closesocket(sockstr);
	if (err == SOCKET_ERROR)
	{

		sprintf(msgErr, "Close TestSignalsocket failed with error: %u\n", WSAGetLastError());
		ssSetErrorStatus(S, msgErr);
        
	}
	WSACleanup();
/* %%%-SFUNWIZ_wrapper_Terminate_Changes_END --- EDIT HERE TO _BEGIN */
}

