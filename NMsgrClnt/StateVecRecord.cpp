#include "stdafx.h"
#include "StateVecRecord.h"


StateVecRecord::StateVecRecord(void)
{
	m_fp = NULL;
	m_aver = NULL;
	m_center = NULL;

	isOpen = false;
}


StateVecRecord::~StateVecRecord(void)
{
}

void StateVecRecord::OpenFile(char *Path, char *filename, char mode){
	char buf[256];
	char buf_aver[256];
	char buf_center[256];

	if(Path == NULL){	
		sprintf(buf, "%s/%s.bin", DEFAULT_FILE_PATH, filename);
		sprintf(buf_aver, "%s/%s_aver.bin", DEFAULT_FILE_PATH, filename);
		sprintf(buf_center, "%s/%s_center.bin", DEFAULT_FILE_PATH, filename);
	}
	else{				
		sprintf(buf, "%s/%s.bin", Path, filename);
		sprintf(buf_aver, "%s/%s_aver.bin", Path, filename);
		sprintf(buf_center, "%s/%s_center.bin", Path, filename);
	}

	if(mode == 'r'){
		m_fp = fopen(buf, "rb");
		m_aver = fopen(buf_aver, "rb");
		m_center = fopen(buf_center, "rb");
	}
	else if(mode == 'w'){
		m_fp = fopen(buf, "wb");
		m_aver = fopen(buf_aver, "wb");
		m_center = fopen(buf_center, "wb");
	}
	else{
		printf("OpenFile mode error!\n");
		return;
	}

	if(m_fp == NULL || m_aver == NULL || m_center == NULL){
		printf("File not found!\n");
		return;
	}

	isOpen = true;
}

void StateVecRecord::CloseFile(){
	isOpen = false;

	if(m_fp != NULL)
		fclose(m_fp);
	
	if(m_aver != NULL)
		fclose(m_aver);

	if(m_center != NULL)
		fclose(m_center);
}

void StateVecRecord::WriteState(StateVector src){
	if(m_fp != NULL && isOpen){
		fwrite(&src, sizeof(StateVector), 1, m_fp);
	}
}

bool StateVecRecord::ReadState(StateVector *dst){
	if(m_fp != NULL){
		fread(dst, sizeof(StateVector), 1, m_fp);
	}
	else return false;

	if(feof(m_fp)){
		printf("End of File!\n");

		isOpen = false;
		fclose(m_fp);

		return false;
	}

	return true;
}

void StateVecRecord::WriteAver(SkeletonInfo src){
	StateVector temp;

	for(int i = 0, j = 0; i < 21; i++){
		if(i == JointType_FootLeft || i == JointType_FootRight)
			continue;

		temp.joints[j].X = src.InfoBody[0].JointPos[i].Position.X;
		temp.joints[j].Y = src.InfoBody[0].JointPos[i].Position.Y;
		temp.joints[j].Z = src.InfoBody[0].JointPos[i].Position.Z;

		j++;
	}

	if(m_fp != NULL && isOpen){
		fwrite(&temp, sizeof(StateVector), 1, m_aver);
	}
}

bool StateVecRecord::ReadAver(StateVector *dst){
	if(m_fp != NULL){
		fread(dst, sizeof(StateVector), 1, m_aver);
	}
	else return false;

	if(feof(m_fp)){
		printf("End of File!\n");
		return false;
	}

	return true;
}

void StateVecRecord::WriteCenter(SkeletonInfo src){
	StateVector temp;

	for(int i = 0, j = 0; i < 21; i++){
		if(i == JointType_FootLeft || i == JointType_FootRight)
			continue;

		temp.joints[j].X = src.InfoBody[0].JointPos[i].Position.X;
		temp.joints[j].Y = src.InfoBody[0].JointPos[i].Position.Y;
		temp.joints[j].Z = src.InfoBody[0].JointPos[i].Position.Z;

		j++;
	}

	if(m_fp != NULL && isOpen){
		fwrite(&temp, sizeof(StateVector), 1, m_center);
	}
}

bool StateVecRecord::ReadCenter(StateVector *dst){
	if(m_fp != NULL){
		fread(dst, sizeof(StateVector), 1, m_center);
	}
	else return false;

	if(feof(m_fp)){
		printf("End of File!\n");
		return false;
	}

	return true;
}

bool StateVecRecord::isOpened(){
	return isOpen;
}