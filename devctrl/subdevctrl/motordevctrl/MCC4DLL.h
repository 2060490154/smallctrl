#ifndef _MYCODE_H_
#define _MYCODE_H_

#include "memory"

#ifdef MCCDLL_EXPORTS
#define EXPORTS_DEMO _declspec( dllexport )
#else
#define EXPORTS_DEMO _declspec(dllimport)
#endif

typedef unsigned short int		McCard_UINT16;
typedef unsigned long			McCard_UINT32;
typedef signed long				McCard_INT32;
typedef signed char				McCard_INT8;
typedef unsigned char			McCard_UINT8;
typedef float					McCard_FP32;
typedef void					McCard_VOID;


class EXPORTS_DEMO MoCtrCard
{
public:
	MoCtrCard();
	~MoCtrCard();

	// RS232 接口
	McCard_UINT16 MoCtrCard_Initial(McCard_UINT8 ComPort);
	McCard_UINT16 MoCtrCard_Unload();

	// TCPIP 接口
	McCard_UINT16 MoCtrCard_Net_Initial(McCard_INT8 *pIPAddr, McCard_INT32 nPort);
	McCard_UINT16 MoCtrCard_GetCommState(McCard_INT32 CommStt[]);

	// 四轴的接口
	McCard_UINT16 MoCtrCard_GetAxisPos(McCard_UINT8 AxisId, McCard_FP32 ResPos[]);
	McCard_UINT16 MoCtrCard_GetAxisSpd(McCard_UINT8 AxisId, McCard_FP32 ResSpd[]);
	McCard_UINT16 MoCtrCard_GetRunState(McCard_INT32 ResStt[]);
	McCard_UINT16 MoCtrCard_GetAdVal(McCard_UINT8 AxisId, McCard_INT32 ResAd[]);
	McCard_UINT16 MoCtrCard_GetOutState(McCard_UINT8 OutGrpIndx, McCard_UINT32 ResOut[]);
	McCard_UINT16 MoCtrCard_GetInputState(McCard_UINT8 InGrpIndx, McCard_UINT32 ResIn[]);
	McCard_UINT16 MoCtrCard_GetIntInputState(McCard_UINT8 InGrpIndx, McCard_UINT32 ResIn[]);
	McCard_UINT16 MoCtrCard_GetCardVersion(McCard_UINT32 ResVer[]);
	McCard_UINT16 MoCtrCard_GetEncoderVal(McCard_UINT8 AxisId, McCard_INT32 EncoderPos[]);
	McCard_UINT16 MoCtrCard_GetBoardHardInfo(McCard_UINT32 HardInfo[]);

	McCard_UINT16 MoCtrCard_SendPara(McCard_UINT8 AxisId, McCard_UINT8 ParaIndx, McCard_FP32 ParaVal);
	McCard_UINT16 MoCtrCard_MCrlAxisMove(McCard_UINT8 AxisId, McCard_INT8 SpdDir);
	McCard_UINT16 MoCtrCard_MCrlAxisRelMove(McCard_UINT8 AxisId, McCard_FP32 DistCmnd, McCard_FP32 VCmnd, McCard_FP32 ACmnd);
	McCard_UINT16 MoCtrCard_MCrlAxisAbsMove(McCard_UINT8 AxisId, McCard_FP32 PosCmnd, McCard_FP32 VCmnd, McCard_FP32 ACmnd);
	McCard_UINT16 MoCtrCard_SeekZero(McCard_UINT8 AxisId, McCard_FP32 VCmnd, McCard_FP32 ACmnd);
	McCard_UINT16 MoCtrCard_CancelSeekZero(McCard_UINT8 AxisId);
	McCard_UINT16 MoCtrCard_ReStartAxisMov(McCard_UINT8 AxisId);
	McCard_UINT16 MoCtrCard_PauseAxisMov(McCard_UINT8 AxisId);
	McCard_UINT16 MoCtrCard_StopAxisMov(McCard_UINT8 AxisId, McCard_FP32 fAcc);
	McCard_UINT16 MoCtrCard_ChangeAxisMovPara(McCard_UINT8 AxisId, McCard_FP32 fVel, McCard_FP32 fAcc);
	McCard_UINT16 MoCtrCard_EmergencyStopAxisMov(McCard_UINT8 AxisId);
	McCard_UINT16 MoCtrCard_QuiteMotionControl(McCard_VOID);

	McCard_UINT16 MoCtrCard_SetOutput(McCard_UINT8 OutputIndex, McCard_UINT8 OutputVal);
	McCard_UINT16 MoCtrCard_SetJoyStickEnable(McCard_UINT8 AxisId, McCard_UINT8 bEnable);
	McCard_UINT16 MoCtrCard_SetAxisRealtiveInputPole(McCard_UINT8 AxisId, McCard_UINT8 InputType, McCard_UINT8 OpenOrClose);
	McCard_UINT16 MoCtrCard_ReadPara(McCard_UINT8 AxisId, McCard_UINT8 ParaIndx, McCard_FP32 ParaVal[]);
	McCard_UINT16 MoCtrCard_ResetCoordinate(McCard_UINT8 AxisId, McCard_FP32 PosRest);
	McCard_UINT16 MoCtrCard_SaveSystemParaToROM(McCard_VOID);
	McCard_UINT16 MoCtrCard_SetEncoderPos(McCard_UINT8 AxisId, McCard_INT32 EncoderPos);
	McCard_UINT16 MoCtrCard_RstZ(McCard_UINT8 AxisId);
	McCard_UINT32 MoCtrCard_GetDLLVersion(McCard_VOID);
	McCard_UINT16 MoCtrCard_MCrlGroupAbsMove(McCard_UINT8 bAxisEn[], McCard_FP32 fPos[], McCard_FP32 fSpd);
	McCard_UINT16 MoCtrCard_MCrlGroupRelMove(McCard_UINT8 bAxisEn[], McCard_FP32 fPos[], McCard_FP32 fSpd);
	McCard_UINT16 MoCtrCard_MCrlGroupAbsMovePTP(McCard_UINT8 bAxisEn[], McCard_FP32 fPos[], McCard_FP32 fSpd[]);
	McCard_UINT16 MoCtrCard_MCrlGroupRelMovePTP(McCard_UINT8 bAxisEn[], McCard_FP32 fDist[], McCard_FP32 fSpd[]);
	McCard_UINT16 MoCtrCard_Test(McCard_INT32 TestVar[]);

private:
	class MoCtrCardImpl;
	std::unique_ptr<MoCtrCardImpl> pimpl;
};

#endif