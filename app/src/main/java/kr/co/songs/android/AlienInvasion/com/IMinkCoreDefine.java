package kr.co.songs.android.AlienInvasion.com;

public interface IMinkCoreDefine {
	
//	static final int D_PUSH_MAIL = 9100;
//	static final int D_PUSH_CUSTOM = 9101;

	static final int E_HTTP_Connection = -1000;
	static final int E_HTTP_SendData = -1001;
	static final int E_HTTP_SendData1 = -1002;
	static final int E_HTTP_SendData2 = -1003;
	static final int E_HTTP_SendDataCMinkML = -1004;
	static final int E_HTTP_SendDataCMinkML1 = -1005;
	static final int E_HTTP_SendDataSyncML = -1006;
	static final int E_HTTP_SendDataSyncML1 = -1007;
	static final int E_HTTP_SendDataSyncML2 = -1008;
	
	static final int E_SYNCML_LoadXML = -1009;
	static final int E_SYNCML_LoadXML1 = -1010;
	static final int E_SYNCML_LoadXML2 = -1011;
	
	static final int E_RM_CRMRecordSet = -1012;
	static final int E_RM_CRMRecordSet1 = -1013;
	static final int E_RM_setTransaction = -1014;
	static final int E_RM_setTransaction1 = -1015;
	
	static final int E_RM_Fetch = -1016;
	static final int E_RM_Fetch1 = -1017;
	
	static final int E_RM_Function = -1018;
	static final int E_RM_Function1 = -1019;
	
	static final int E_RM_Fetch2 = -1020;
	static final int E_RM_Fetch3 = -1021;
	static final int E_RM_Fetch4 = -1022;
	static final int E_RM_Fetch5 = -1023;
	
	static final int E_HTTP_Execute = -1024;
	static final int E_HTTP_Execute1 = -1025;
	static final int E_HTTP_Execute2 = -1026;
	static final int E_HTTP_Execute3 = -1027;
	
	static final int E_SVCEND_getResultCode = -1028;
	static final int E_SVCEND_getResultCode1 = -1029;
	
	static final int E_ML_LoadXml = -1030;
	
	static final int E_NEXTREQ_getNextStatusCmd = -1031;
	static final int E_ENG_ChangedConfig = -1032;
	static final int E_ENG_RequestEnd = -1033;
	static final int E_ENG_ZeroSize = -1034;
	
	static final int E_ENG_CheckStatus = -1035;
	static final int E_ENG_CheckStatus1 = -1036;
	
	static final int E_RM_getData = -1037;
	static final int E_ML_recode = -1038;
	static final int E_ML_Load = -1039;
	
	static final int E_MM_MailMSGParser = -1040;
	static final int E_MM_MailMSGParser1 = -1041;
	static final int E_MM_MailMSGParser2 = -1042;
	
	static final int E_MV_save = -1043;
	static final int E_SETTING_setLastAnchor = -1044;
	
	static final int E_MMDBMAN_NewByServer = -1045;
	static final int E_MMDBMAN_NewByServer1 = -1046;
	static final int E_MMDBMAN_NewByServer2 = -1047;
	static final int E_MMDBMAN_NewByServer3 = -1048;
	
	static final int E_MMDBMAN_UpdateByServer = -1049;
	static final int E_MMDBMAN_getStartNewItem = -1050;
	static final int E_MMDBMAN_getStartUpdateItem = -1060;
	static final int E_MMDBMAN_getStartDeleteItem = -1061;
	static final int E_MMDBMAN_GetAllItem = -1062;
	static final int E_MMDBMAN_GetNotReadMailCount = -1063;
	static final int E_MMDBMAN_GetMinkMailInfo = -1064;
	
	static final int E_MMDBMAN_RcvNewItem = -1065;
	static final int E_MMDBMAN_RcvUpdateItem = -1066;
	static final int E_MMDBMAN_LoadXml = -1067;
	static final int E_MMDBMAN_LoadXml1 = -1068;
	
	static final int E_MMDBMAN_MailToMailInfo = -1069;
	static final int E_MMDBMAN_MailToMailInfoBt = -1070;
	static final int E_MMDBMAN_SaveSendMail = -1071;
	static final int E_MMDBMAN_SaveSendMail1 = -1072;
	static final int E_MMDBMAN_SaveSendMail2 = -1073;
	static final int E_MMDBMAN_DeleteItem = -1074;
	static final int E_MMDBMAN_DeleteItemSoft = -1075;
	static final int E_MMDBMAN_UpdateMail = -1076;
	static final int E_MMDBMAN_DbToSyncUnitByNewItem = -1077;
	static final int E_MMDBMAN_DbToSyncUnitByUpdateItem = -1078;
	static final int E_MMDBMAN_DeleteAllItem = -1079;
	
	static final int E_MMDBMAN_CreateDB = -1080;
	
	
	static final int E_VIEWMAN_ChangeView = -1090;
	
	static final int E_ENG_ResponseStatus = -1100;
	static final int E_ENG_ResponseStatus1 = -1101;
	static final int E_ENG_ResponseStatus2 = -1102;
	static final int E_ENG_ResponseStatus3 = -1103;
	
	static final int E_MAILUTIL_ComposeMail = -1110;
	static final int E_MAILUTIL_AddAdress = -1111;
	static final int E_MAILUTIL_UtcToTimestamp = -1112;
	static final int E_MAILUTIL_readFile = -1113;
	static final int E_MAILUTIL_printMessage = -1114;
	
	
	static final int E_MPARSER_getAllPartsOfMessage = -1120;
	static final int E_MPARSER_Error1 = -1121;
	static final int E_MPARSER_Error2 = -1122;
	static final int E_MPARSER_createPartsList = -1123;
	static final int E_MPARSER_createPartsList1 = -1124;
	static final int E_MPARSER_createPartsList2 = -1125;
	static final int E_MPARSER_createPartsList3 = -1126;
	static final int E_MPARSER_createPartsList4 = -1127;
	static final int E_MPARSER_createPartialPartsList = -1128;
	static final int E_MPARSER_createcheckBodyTxt = -1129;
	static final int E_MPARSER_checkBodyHtml = -1130;
	
	static final int E_Frm_handleMessage = -1131;
	
	static final int E_Handler_Message_Code_Login_ID_Password = -302;
	static final int E_Handler_Message_Code_Login_Stop = -303;
	static final int E_Handler_Message_Code_Login_Lost = -304;
	static final int E_Handler_Message_Code_MoinApi = -106;
	
	
	
	static final int E_ENG_ConnecotrSync= -1200;
	
	
	
	
	
	
	
}
