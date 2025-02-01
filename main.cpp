#include <GL/glew.h>
#include <GL/glut.h>
#include <FreeImage.h>
#include <cmath>
#include <cstdio>

//variabel untuk texture keranjang
GLuint texture_Keranjang_ID;

float rotationAngle = 0.0f;
bool isRotating = false; 

// Variabel untuk translasi dan skala
float translateY = 0.0f;
float translateX = 0.0f; 
float scaleFactor = 1.0f;

float cameraX = 0.0f, cameraY = 5.0f, cameraZ = 15.0f;
float cameraTargetX = 0.0f, cameraTargetY = 0.0f, cameraTargetZ = 0.0f;

// Fungsi membuat keranjang
void createBasketWithHandle() {
    GLUquadric* quadric = gluNewQuadric();
    gluQuadricTexture(quadric, GL_TRUE);
    gluQuadricNormals(quadric, GLU_SMOOTH);

    // Dasar keranjang berbentuk piring
    glPushMatrix();
    glTranslatef(0.0, -2.0, 0.0);
    glRotatef(90, 1.0, 0.0, 0.0);
    gluDisk(quadric, 0.0, 2.5, 100, 1);
    glPopMatrix();

    // Dinding keranjang
    glPushMatrix();
    glRotatef(90, 1.0, 0.0, 0.0);
    gluCylinder(quadric, 3.5, 2.5, 2.0, 100, 100);
    glPopMatrix();
    
    // Pegangan keranjang melengkung sempurna
    glPushMatrix();
    float radius = 3.4f; // Jari-jari pegangan
    float thickness = 0.2f; // Ketebalan pegangan
    int segments = 180; // Jumlah segmen untuk membuat pegangan mulus / kemulusan pegangan
    
    // Buat pegangan sebagai tabung melengkung
    for (int i = 0; i <= segments; i++) {
        float angle = M_PI * i / segments; // Sudut setiap segmen (0 hingga PI)
        float x = radius * cos(angle);    // Koordinat X berdasarkan lingkaran
        float y = radius * sin(angle);    // Koordinat Y berdasarkan lingkaran

        glPushMatrix();
        glTranslatef(x, y, 0.0);         // Posisi segmen
        glRotatef(-angle * 180 / M_PI, 0.0, 0.0, 1.0); // Rotasi agar melengkung
        gluCylinder(quadric, thickness, thickness, 0.5, 30, 30); // Silinder kecil
        glPopMatrix();
    }
    glPopMatrix();
}

void updateCamera() {
    glLoadIdentity();
    gluLookAt(cameraX, cameraY, cameraZ, cameraTargetX, cameraTargetY, cameraTargetZ, 0.0, 1.0, 0.0);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(0.0, 5.0, 15.0, 0.0, -1.0, 0.0, 0.0, 1.0, 0.0);
    updateCamera();

    // Rotasi seluruh objek jika diaktifkan
    if (isRotating) {
        glRotatef(rotationAngle, 0.0, 1.0, 0.0);
    }

    // Translasi dan skala seluruh objek
    glTranslatef(translateX, translateY, 0.0f);
    glScalef(scaleFactor, scaleFactor, scaleFactor);
	
	glEnable(GL_LIGHTING); //hidupkan cahaya
	
	glTranslatef(0.0, -0.7, 0.0); // Posisi keranjang
	glBindTexture(GL_TEXTURE_2D, texture_Keranjang_ID);
    createBasketWithHandle();
	    
    glDisable(GL_LIGHTING); // nonaktifan cahaya
    glutSwapBuffers();
}

// Fungsi untuk memuat tekstur
GLuint loadTexture(const char* path) {
    GLuint textureID;
    glGenTextures(1, &textureID);

    void* imgData;
    int imgWidth, imgHeight;

    FREE_IMAGE_FORMAT format = FreeImage_GetFIFFromFilename(path);
    if (format == FIF_UNKNOWN) {
        printf("Unknown file type for texture image file %s\n", path);
        return 0;
    }

    FIBITMAP* bitmap = FreeImage_Load(format, path, 0);
    if (!bitmap) {
        printf("Failed to load image %s\n", path);
        return 0;
    }

    FIBITMAP* bitmap2 = FreeImage_ConvertTo24Bits(bitmap);
    FreeImage_Unload(bitmap);
    imgData = FreeImage_GetBits(bitmap2);
    imgWidth = FreeImage_GetWidth(bitmap2);
    imgHeight = FreeImage_GetHeight(bitmap2);

    if (imgData) {
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgWidth, imgHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, imgData);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        FreeImage_Unload(bitmap2);
    } else {
        printf("Failed to get texture data from %s\n", path);
        FreeImage_Unload(bitmap2);
    }

    return textureID;
}

// Fungsi timer untuk animasi rotasi
void updateRotation(int value) {
    if (isRotating) {
        rotationAngle += 0.5f;
        if (rotationAngle >= 360.0f) {
            rotationAngle -= 360.0f;
        }
    }
    glutPostRedisplay();
    glutTimerFunc(16, updateRotation, 0);
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'r': // Toggle rotasi
            isRotating = !isRotating;
            break;
        case 'w': // Translasi ke atas
            translateY += 0.1f;
            break;
        case 'z': // Translasi ke bawah
            translateY -= 0.1f;
            break;
        case 'a': // Translasi ke kiri
            translateX -= 0.1f;
            break;
        case 's': // Translasi ke kanan
            translateX += 0.1f;
            break;
        case '+': // Perbesar skala
            scaleFactor += 0.1f;
            break;
        case '-': // Perkecil skala
            scaleFactor -= 0.1f;
            break;            
        case 'i': cameraY += 0.5f; break; // Geser kamera ke atas
        case 'm': cameraY -= 0.5f; break; // Geser kamera ke bawah
        case 'j': cameraX -= 0.5f; break; // Geser kamera ke kiri
        case 'k': cameraX += 0.5f; break; // Geser kamera ke kanan
        case 'v': cameraZ += 0.5f; break; // Dekatkan kamera
        case 'b': cameraZ -= 0.5f; break; // Jauhkan kamera
        case 27: // Tombol Esc untuk keluar
            exit(0);
            break;
    }
    glutPostRedisplay();
}

void initProjection() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    // Inisialisasi proyeksi
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 16.0 / 9.0, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);

    // Konfigurasi pencahayaan
    glEnable(GL_LIGHT0); // Aktifkan lampu sumber cahaya pertama
    GLfloat lightAmbient[] = { 2.0f, 2.0f, 2.0f, 2.0f }; // Cahaya lingkungan (ambient)
    GLfloat lightDiffuse[] = { 2.0f, 2.0f, 1.0f, 1.0f }; // Cahaya tersebar (diffuse)
    GLfloat lightSpecular[] = { 2.0f, 1.0f, 1.0f, 2.0f }; // Cahaya spesular (highlight)
    GLfloat lightPosition[] = { 5.0f, 10.0f, 10.0f, 1.0f }; // Posisi sumber cahaya

    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    // Memuat tekstur
    texture_Keranjang_ID = loadTexture("textureObjek/keranjang3.png");
}

// Main
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(1280, 720);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("2306142_Muhamad Saepul Rizal_Objek Keranjang_UAS Grafkom");

    glewInit();
    initProjection();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(16, updateRotation, 0);
    glutMainLoop();
    return 0;
}

