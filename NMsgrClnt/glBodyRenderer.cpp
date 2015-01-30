#include "stdafx.h"
#include "glBodyRenderer.h"


glBodyRenderer::glBodyRenderer(void)
{
	view_rotx = 10.f, view_roty = 180.f, view_rotz = 0.f;
	view_tranx = 0.0f, view_trany = -5.0f, view_tranz = -40.f;

	////set Identity - Initialize
	//TransformMat = (cv::Mat_<float>::eye(4,4));

	//Thread Run!
	m_EndThread = false;
	m_EnableThread = false;
	InitializeCriticalSection(&m_cs);

	m_InitCheck = false;

}


glBodyRenderer::~glBodyRenderer(void)
{
	m_EnableThread = false;
	free(m_BodyInfo);
}

void glBodyRenderer::InitializeRenderer(char *name, int xpos, int ypos){

	strcpy(WindowName, name);
	winX = xpos;
	winY = ypos;

	printf("Start rendering thread..\n");
	m_BodyInfo = (StateVector*)malloc(sizeof(StateVector));
	m_EnableThread = true;
	//m_glThread.StartThread(renderThread, this);

	//Thread Initialize..
	printf("Thread Initialize start...\n");
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		exit( EXIT_FAILURE );
	}

	glfwWindowHint(GLFW_DEPTH_BITS, 160);

	m_window = glfwCreateWindow( 640, 480, WindowName, NULL, NULL );
	glfwSetWindowPos(m_window, winX, winY);
	//glfwSetWindowUserPointer(m_window, this);
	if (!m_window)
	{
		fprintf( stderr, "Failed to open GLFW window\n" );
		glfwTerminate();
		exit( EXIT_FAILURE );
	}

	// Set callback functions

	glfwSetFramebufferSizeCallback(m_window, reshape);
	glfwSetKeyCallback(m_window, key);
	glfwSetScrollCallback(m_window, scroll_callback);

	glfwMakeContextCurrent(m_window);
	glfwSwapInterval( 1 );

	glfwGetFramebufferSize(m_window, &m_width, &m_height);
	reshape(m_window, m_width, m_height);

	// Parse command-line options
	init();
	printf("Thread Initialize complete!\n");
	m_InitCheck = true;
}

int glBodyRenderer::CheckWindowClose(){
	return /*glfwWindowShouldClose(m_window)*/m_EndThread;
}

void glBodyRenderer::DeInitializeRenderer(){
	// Terminate GLFW
	glfwTerminate();

	//Thread Kill
	m_EnableThread = false;

	// Exit program
	exit( EXIT_SUCCESS );
}

/* change view angle, exit upon ESC */
void glBodyRenderer::key( GLFWwindow* window, int k, int s, int action, int mods )
{
	glBodyRenderer* tClass = reinterpret_cast<glBodyRenderer *>(glfwGetWindowUserPointer(window));
	switch (k) {
	case GLFW_KEY_Z:
		if( mods & GLFW_MOD_SHIFT )
			tClass->view_rotz -= 5.0;
		else
			tClass->view_rotz += 5.0;
		break;
	case GLFW_KEY_ESCAPE:
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
			tClass->m_EnableThread = false;
			break;
		}
	case GLFW_KEY_UP:
		tClass->view_rotx += 5.0;
		break;
	case GLFW_KEY_DOWN:
		tClass->view_rotx -= 5.0;
		break;
	case GLFW_KEY_LEFT:
		tClass->view_roty += 5.0;
		break;
	case GLFW_KEY_RIGHT:
		tClass->view_roty -= 5.0;
		break;
	case GLFW_KEY_SPACE:
		tClass->view_rotx = 20.f, tClass->view_roty = 180.f, tClass->view_rotz = 0.f;
		break;
	default:
		return;
	}
}

void glBodyRenderer::scroll_callback(GLFWwindow* window, double x, double y){
	static const float tran_acc = 2.0f;
	glBodyRenderer* tClass = reinterpret_cast<glBodyRenderer *>(glfwGetWindowUserPointer(window));
	//view_tranx += y;
	tClass->view_trany += tran_acc/8.f*y;
	tClass->view_tranz += tran_acc*y;
}

/* new window size */
void glBodyRenderer::reshape( GLFWwindow* window, int width, int height )
{
	GLfloat h = (GLfloat) height / (GLfloat) width;
	GLfloat xmax, znear, zfar;

	znear = 5.0f;
	zfar  = 200.0f;
	xmax  = znear * 0.5f;

	glViewport( 0, 0, (GLint) width, (GLint) height );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glFrustum( -xmax, xmax, -xmax*h, xmax*h, znear, zfar );
	//glMatrixMode( GL_MODELVIEW );				//ModelView가 카메라
	//glLoadIdentity();
	//glTranslatef( 0.0, 0.0, -40.0 );			// 카메라 관련?
}

/*********************************************************************************
* Draw x, y, z axis of current frame on screen.
* x, y, and z are corresponded Red, Green, and Blue, resp.
**********************************************************************************/
void glBodyRenderer::drawFrame(float len)
{
	glDisable(GL_LIGHTING);		// Lighting is not needed for drawing axis.
	glBegin(GL_LINES);			// Start drawing lines.
	glColor3d(1,0,0);			// color of x-axis is red.
	glVertex3d(0,0,0);			
	glVertex3d(len,0,0);		// Draw line(x-axis) from (0,0,0) to (len, 0, 0). 
	glColor3d(0,1,0);			// color of y-axis is green.
	glVertex3d(0,0,0);			
	glVertex3d(0,len,0);		// Draw line(y-axis) from (0,0,0) to (0, len, 0).
	glColor3d(0,0,1);			// color of z-axis is  blue.
	glVertex3d(0,0,0);
	glVertex3d(0,0,len);		// Draw line(z-axis) from (0,0,0) - (0, 0, len).
	glEnd();					// End drawing lines.
}

/*********************************************************************************
* Draw floor on 3D plane.
**********************************************************************************/
void glBodyRenderer::drawFloor()
{  
	glDisable(GL_LIGHTING);
	// Set color of the floor.
	// Assign checker-patterned texture.
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_floorTexID );

	// Draw the floor. Match the texture's coordinates and the floor's coordinates resp. 
	glBegin(GL_POLYGON);
	glTexCoord2d(0,0);
	glVertex3d(-12,0,-12);		// Texture's (0,0) is bound to (-12,-0.1,-12).
	glTexCoord2d(1,0);
	glVertex3d( 12,0,-12);		// Texture's (1,0) is bound to (12,-0.1,-12).
	glTexCoord2d(1,1);
	glVertex3d( 12,0, 12);		// Texture's (1,1) is bound to (12,-0.1,12).
	glTexCoord2d(0,1);
	glVertex3d(-12,0, 12);		// Texture's (0,1) is bound to (-12,-0.1,12).
	glEnd();

	glDisable(GL_TEXTURE_2D);	
	drawFrame(5);				// Draw x, y, and z axis.
}

void glBodyRenderer::drawSphere(double r, int lats, int longs, GLfloat *pColor) {
	int i, j;

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, pColor);
	for(i = 0; i <= lats; i++) {
		double lat0 = PI * (-0.5 + (double) (i - 1) / lats);
		double z0  = r*sin(lat0);
		double zr0 =  r*cos(lat0);

		double lat1 = PI * (-0.5 + (double) i / lats);
		double z1 = r*sin(lat1);
		double zr1 = r*cos(lat1);

		glBegin(GL_QUAD_STRIP);
		for(j = 0; j <= longs; j++) {
			double lng = 2 * PI * (double) (j - 1) / longs;
			double x = cos(lng);
			double y = sin(lng);

			glNormal3f(x * zr0, y * zr0, z0);
			glVertex3f(x * zr0, y * zr0, z0);
			glNormal3f(x * zr1, y * zr1, z1);
			glVertex3f(x * zr1, y * zr1, z1);
		}
		glEnd();
	}
}

/* program & OpenGL initialization */
void glBodyRenderer::init()
{

	static GLfloat pos[4] = {5.f, 5.f, 10.f, 0.f};
	static GLfloat red[4] = {0.8f, 0.1f, 0.f, 1.f};
	static GLfloat green[4] = {0.f, 0.8f, 0.2f, 1.f};
	static GLfloat blue[4] = {0.2f, 0.2f, 1.f, 1.f};

	// Set up OpenGL state
	glShadeModel(GL_SMOOTH);         // Set Smooth Shading
	glEnable(GL_DEPTH_TEST);         // Enables Depth Testing
	glDepthFunc(GL_LEQUAL);          // The Type Of Depth Test To Do
	// Use perspective correct interpolation if available
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	// Define lighting for the scene
	float lightDirection[]   = {1.0, 1.0, 1.0, 0};
	float ambientIntensity[] = {0.1, 0.1, 0.1, 1.0};
	float lightIntensity[]   = {0.9, 0.9, 0.9, 1.0};
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientIntensity);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightIntensity);
	glLightfv(GL_LIGHT0, GL_POSITION, lightDirection);
	glEnable(GL_LIGHT0);

	// initialize floor
	{
		// After making checker-patterned texture, use this repetitively.

		// Insert color into checker[] according to checker pattern.
		const int size = 8;
		unsigned char checker[size*size*3];
		for( int i=0; i < size*size; i++ )
		{
			if (((i/size) ^ i) & 1)
			{
				checker[3*i+0] = 200;
				checker[3*i+1] = 32;
				checker[3*i+2] = 32;
			}
			else
			{
				checker[3*i+0] = 200;
				checker[3*i+1] = 200;
				checker[3*i+2] = 32;
			}
		}

		// Make texture which is accessible through floorTexID. 
		glGenTextures( 1, &m_floorTexID );				
		glBindTexture(GL_TEXTURE_2D, m_floorTexID);		
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, checker);
	}

	glEnable(GL_NORMALIZE);
}

void glBodyRenderer::Display(StateVector *JointData)
{
	// Draw
	draw(JointData);

	// Swap buffers
	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

/* OpenGL draw function & timing */
//거꾸로 생각해야되네
void glBodyRenderer::draw(StateVector *JointData)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	glTranslatef(0.0f, -9.5f, 27.5f);
	drawFloor();
	glPopMatrix();

	glEnable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);
	////glShadeModel(GL_FLAT);
	//drawSphere(1.0, 10, 10, green);

	//Make Info Body
	if(JointData != NULL){
		BodyInfo temp;

		temp.BodyID = 43;
		for(int i = 0, j = 0; i < 21; i++){
			if(i == JointType_FootRight || i == JointType_FootLeft)
				continue;
			temp.JointPos[i].Position.X = JointData->joints[j].X;
			temp.JointPos[i].Position.Y = JointData->joints[j].Y;
			temp.JointPos[i].Position.Z = JointData->joints[j].Z;
			j++;
		}

		drawBody(temp);
	}


	//drawFrame(5);
	glMatrixMode( GL_MODELVIEW );				//ModelView가 카메라
	glLoadIdentity();
	glTranslatef( view_tranx, view_trany, view_tranz );			// 카메라 관련?
	glRotatef(view_rotx, 1.0, 0.0, 0.0);
	glRotatef(view_roty, 0.0, 1.0, 0.0);
	glRotatef(view_rotz, 0.0, 0.0, 1.0);
	glTranslatef(0.0f, 10.0f, -30.0f);
}

void glBodyRenderer::DrawSkelBone(Joint* pJoints, JointType joint0, JointType joint1, GLfloat *t_Color){
	static const float sphereRad = .2;
	static const float cylinderRad = .3;
	static const int tSlice = 10;

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, t_Color);

	CameraSpacePoint temp;
	temp.X = pJoints[joint1].Position.X - pJoints[joint0].Position.X;
	temp.Y = pJoints[joint1].Position.Y - pJoints[joint0].Position.Y;
	temp.Z = pJoints[joint1].Position.Z - pJoints[joint0].Position.Z;

	//handle the degenerate case of z1 == z2 with an approximation
	if(temp.Z == 0)
		temp.Z = .0001;

	float v = sqrt( temp.X*temp.X + temp.Y*temp.Y + temp.Z*temp.Z );
	float ax = 57.2957795*acos( temp.Z/v );
	if ( temp.Z < 0.0 )
		ax = -ax;
	float rx = -temp.Y*temp.Z;
	float ry = temp.X*temp.Z;

	GLUquadric *quadric;
	quadric = gluNewQuadric();

	glPushMatrix();
	glTranslatef(pJoints[joint0].Position.X, pJoints[joint0].Position.Y, pJoints[joint0].Position.Z);
	gluSphere(quadric, sphereRad, tSlice, tSlice);
	glRotatef(ax, rx, ry, 0.0);
	gluQuadricOrientation(quadric,GLU_OUTSIDE);
	gluCylinder(quadric, cylinderRad, cylinderRad, v, tSlice, tSlice);
	glPopMatrix();

	gluDeleteQuadric(quadric);
}

void glBodyRenderer::drawBody(BodyInfo tBody){
	UINT64 tBodyID = tBody.BodyID;
	GLfloat tColor[4] = {(tBodyID*37)%256/255.f, (tBodyID*113)%256/255.f, (tBodyID*71)%256/255.f};
	GLfloat tLeftColor[4] = {1.0f, 1.0f, 1.0f};
	/*static const float sphereRad = .2;
	static const float cylinderRad = .1;
	static const int tSlice = 10;*/

	/*drawSphere(2., 10, 10, tColor);*/
	glPushMatrix();

	/*GLUquadric *quadric;
	quadric = gluNewQuadric();*/
	/*glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, tColor);*/

	/////////////////////Draw Joint////////////////////////////////////
	for(int i = 0; i < JointType_Count; i++){
		glTransformCoordinate(&tBody.JointPos[i]);			//Transformation -> Image coordinate

		//glPushMatrix();
		//glTranslatef(gljointpoints[i].x, gljointpoints[i].y, gljointpoints[i].z);
		////printf("%f %f %f\n", gljointpoints[i].x, gljointpoints[i].y, gljointpoints[i].z);
		////printf("%f %f %f\n", tBody.JointPos[i].Position.X, tBody.JointPos[i].Position.Y, tBody.JointPos[i].Position.Z);
		//gluSphere(quadric, sphereRad, tSlice, tSlice);
		//glPopMatrix();
	}
	// Torso
	DrawSkelBone(tBody.JointPos, JointType_Head, JointType_Neck, tColor);
	DrawSkelBone(tBody.JointPos, JointType_Neck, JointType_SpineShoulder, tColor);
	DrawSkelBone(tBody.JointPos, JointType_SpineShoulder, JointType_SpineMid, tColor);
	DrawSkelBone(tBody.JointPos, JointType_SpineMid, JointType_SpineBase, tColor);
	DrawSkelBone(tBody.JointPos, JointType_SpineShoulder, JointType_ShoulderRight, tColor);
	DrawSkelBone(tBody.JointPos, JointType_SpineShoulder, JointType_ShoulderLeft, tColor);
	DrawSkelBone(tBody.JointPos, JointType_SpineBase, JointType_HipRight, tColor);
	DrawSkelBone(tBody.JointPos, JointType_SpineBase, JointType_HipLeft, tColor);

	// Right Arm
	DrawSkelBone(tBody.JointPos, JointType_ShoulderRight, JointType_ElbowRight, tColor);
	DrawSkelBone(tBody.JointPos, JointType_ElbowRight, JointType_WristRight, tColor);
	//DrawSkelBone(tBody.JointPos, gljointpoints, JointType_WristRight, JointType_HandRight, tColor);
	//DrawSkelBone(tBody.JointPos, gljointpoints, JointType_HandRight, JointType_HandTipRight, tColor);
	//DrawSkelBone(tBody.JointPos, gljointpoints, JointType_WristRight, JointType_ThumbRight, tColor);

	// Left Arm
	DrawSkelBone(tBody.JointPos, JointType_ShoulderLeft, JointType_ElbowLeft, tLeftColor);
	DrawSkelBone(tBody.JointPos, JointType_ElbowLeft, JointType_WristLeft, tLeftColor);
	//DrawSkelBone(tBody.JointPos, gljointpoints, JointType_WristLeft, JointType_HandLeft, tColor);
	//DrawSkelBone(tBody.JointPos, gljointpoints, JointType_HandLeft, JointType_HandTipLeft, tColor);
	//DrawSkelBone(tBody.JointPos, gljointpoints, JointType_WristLeft, JointType_ThumbLeft, tColor);

	// Right Leg
	DrawSkelBone(tBody.JointPos, JointType_HipRight, JointType_KneeRight, tColor);
	DrawSkelBone(tBody.JointPos, JointType_KneeRight, JointType_AnkleRight, tColor);
	//DrawSkelBone(tBody.JointPos, gljointpoints, JointType_AnkleRight, JointType_FootRight, tColor);

	// Left Leg
	DrawSkelBone(tBody.JointPos, JointType_HipLeft, JointType_KneeLeft, tLeftColor);
	DrawSkelBone(tBody.JointPos, JointType_KneeLeft, JointType_AnkleLeft, tLeftColor);
	//DrawSkelBone(tBody.JointPos, gljointpoints, JointType_AnkleLeft, JointType_FootLeft, tColor);
	///////////////////////////////////////////////////////////////////

	glPopMatrix();


}

void glBodyRenderer::glTransformCoordinate(Joint *src){
	static const float scale = 5.f;

	//cv::Mat t_src = (cv::Mat_<float>(4,1) << src.Position.X * 10.f, src.Position.Y * 10.f, src.Position.Z * 10.f, 1.);
	//cv::Mat t_dst = (cv::Mat_<float>(4,1) << src.Position.X * 10.f, src.Position.Y * 10.f, src.Position.Z * 10.f, 1.);	//initialize

	////printf("%d %d\n", TransformMat.rows, TransformMat.cols);
	//t_dst = TransformMat*t_src;

	//float _w = t_dst.at<float>(3,0);
	////printf("_w : %f\n", _w);
	//dst->x = t_dst.at<float>(0,0) / _w;
	//dst->y = t_dst.at<float>(1,0) / _w;
	//dst->z = t_dst.at<float>(2,0) / _w;
	src->Position.X *= 10.f;
	src->Position.Y *= 10.f;
	src->Position.Z *= 10.f;
}

void glBodyRenderer::SetBodyInfo(StateVector *srcBodyInfo){
	/*EnterCriticalSection(&m_cs);
	memcpy(m_BodyInfo, srcBodyInfo, sizeof(StateVector));
	LeaveCriticalSection(&m_cs);*/

	if(!glfwWindowShouldClose(m_window) || m_EnableThread){ 

		/*EnterCriticalSection(&t_glBodyRenderer->m_cs);
		memcpy(&threadBodyInfo, t_glBodyRenderer->m_BodyInfo, sizeof(StateVector)); 
		LeaveCriticalSection(&t_glBodyRenderer->m_cs);*/

		Display(srcBodyInfo);
	}
}

UINT WINAPI glBodyRenderer::renderThread(LPVOID param){
	glBodyRenderer *t_glBodyRenderer = (glBodyRenderer *)param;
	//SkeletonInfo *threadBodyInfo;
	StateVector threadBodyInfo;

	//threadBodyInfo = (SkeletonInfo*)malloc(sizeof(SkeletonInfo)*t_glBodyRenderer->m_numKinect);

	//Thread Initialize..
	printf("Thread Initialize start...\n");
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		exit( EXIT_FAILURE );
	}

	glfwWindowHint(GLFW_DEPTH_BITS, 160);

	t_glBodyRenderer->m_window = glfwCreateWindow( 640, 480, t_glBodyRenderer->WindowName, NULL, NULL );
	glfwSetWindowPos(t_glBodyRenderer->m_window, t_glBodyRenderer->winX, t_glBodyRenderer->winY);
	glfwSetWindowUserPointer(t_glBodyRenderer->m_window, t_glBodyRenderer);
	if (!t_glBodyRenderer->m_window)
	{
		fprintf( stderr, "Failed to open GLFW window\n" );
		glfwTerminate();
		exit( EXIT_FAILURE );
	}

	// Set callback functions

	glfwSetFramebufferSizeCallback(t_glBodyRenderer->m_window, t_glBodyRenderer->reshape);
	glfwSetKeyCallback(t_glBodyRenderer->m_window, t_glBodyRenderer->key);
	glfwSetScrollCallback(t_glBodyRenderer->m_window, scroll_callback);

	glfwMakeContextCurrent(t_glBodyRenderer->m_window);
	glfwSwapInterval( 1 );

	glfwGetFramebufferSize(t_glBodyRenderer->m_window, &t_glBodyRenderer->m_width, &t_glBodyRenderer->m_height);
	reshape(t_glBodyRenderer->m_window, t_glBodyRenderer->m_width, t_glBodyRenderer->m_height);

	// Parse command-line options
	t_glBodyRenderer->init();
	printf("Thread Initialize complete!\n");
	t_glBodyRenderer->m_InitCheck = true;

	//Thread run...
	while(!glfwWindowShouldClose(t_glBodyRenderer->m_window) || t_glBodyRenderer->m_EnableThread){ 

		EnterCriticalSection(&t_glBodyRenderer->m_cs);
		memcpy(&threadBodyInfo, t_glBodyRenderer->m_BodyInfo, sizeof(StateVector)); 
		LeaveCriticalSection(&t_glBodyRenderer->m_cs);

		t_glBodyRenderer->Display(&threadBodyInfo);
	}

	//free(t_glBodyRenderer);

	//Thread exit...
	t_glBodyRenderer->m_EndThread = true;

	return 0;
}

void glBodyRenderer::WaitUntilThreadDead(){
	m_EnableThread = false;

	while(m_EndThread){
		Sleep(100);
	}
}

void glBodyRenderer::WaitUntilThreadInit(){
	while(!m_InitCheck){
		Sleep(10);
	}
}