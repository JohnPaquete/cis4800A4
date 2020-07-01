
/* Derived from scene.c in the The OpenGL Programming Guide */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Linux OpenGL Headers

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>


// MacOS OpenGL Headers
/*
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
*/

	/* used to rotate object in update() */
float rot = 0.0;
float r = 1.0;
float zoom = -15.0;

//ply model data structures
typedef struct vertex {
   float x;
   float y;
   float z;
} Vertex;

typedef struct face {
   int size;
   float nx;
   float ny;
   float nz;
   int * vertexes;
} Face;

typedef struct model {
   int vertexCount;
   int faceCount;
   Vertex * vertexes;
   Face * faces;
} Model;

Model model;

	/* calculate the length of a vector */
float length(float *x, float *y, float *z) {
   return( sqrtf( (*x * *x) + (*y * *y) + (*z * *z)) );
}

	/* creates a unit vector */
	/* divide a vector by its own length */
void normalize(float *xd, float *yd, float *zd) {
float len;
   len = length(xd, yd, zd);
   *xd = *xd / len;
   *yd = *yd / len;
   *zd = *zd / len;
}


/*  Initialize material property and light source.  */
void init (void) {
   GLfloat light_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
   GLfloat light_diffuse[] = { 0.7, 0.7, 0.7, 1.0 };
   GLfloat light_specular[] = { 0.7, 0.7, 0.7, 1.0 };
   GLfloat light_ambient1[] = { 0.2, 0.2, 0.2, 1.0 };
   GLfloat light_diffuse1[] = { 0.4, 0.4, 0.4, 1.0 };
   GLfloat light_specular1[] = { 0.9, 0.9, 0.9, 1.0 };
   GLfloat light_full_off[] = {0.0, 0.0, 0.0, 1.0};
   GLfloat light_full_on[] = {1.0, 1.0, 1.0, 1.0};

   GLfloat light_position[] = { 10.0, 10.0, 10.0, 0.0 };
   GLfloat light_position2[] = { -5.0, -5.0, -5.0, 0.0 };

	/* if lighting is turned on then use ambient, diffuse and specular
	   lights, otherwise use ambient lighting only */
   glLightfv (GL_LIGHT0, GL_AMBIENT, light_ambient);
   glLightfv (GL_LIGHT0, GL_DIFFUSE, light_diffuse);

   glLightfv (GL_LIGHT1, GL_AMBIENT, light_ambient1);
   glLightfv (GL_LIGHT1, GL_DIFFUSE, light_diffuse1);
   glLightfv (GL_LIGHT0, GL_POSITION, light_position);
   glLightfv (GL_LIGHT1, GL_POSITION, light_position2);
   
   glEnable (GL_LIGHTING);
   glEnable (GL_LIGHT0);
   glEnable (GL_LIGHT1);
   glEnable(GL_DEPTH_TEST);
}


void display (void)
{
   GLfloat blue[]  = {0.0, 0.0, 1.0, 1.0};
   GLfloat brown[]   = {0.6, 0.2, 0.0, 1.0};
   GLfloat darkbrown[]   = {0.3, 0.1, 0.0, 1.0};
   GLfloat green[] = {0.0, 1.0, 0.0, 1.0};
   GLfloat white[] = {1.0, 1.0, 1.0, 1.0};
   GLfloat gray[] = {0.8, 0.8, 0.8, 1.0};
   GLfloat darkgray[] = {0.3, 0.3, 0.3, 1.0};


   glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* draw surfaces as smooth shaded */
	/* do not change this to GL_FLAT, use normals for flat shading */
   glShadeModel(GL_SMOOTH);

	/* draw polygons as either solid or outlines */
   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	/* give all objects the same shininess value */
   glMaterialf(GL_FRONT, GL_SHININESS, 96);

	/* set starting location of objects */
   glPushMatrix ();
   glTranslatef(0.0, 0.0, zoom);
	/* rotate around the y axis, angle or rotation (rot) modified in
	   the update() function */
   glRotatef (rot, 0.0, 1.0, 0.0);

	/* set polygon colour */
   glMaterialfv(GL_FRONT, GL_AMBIENT, darkbrown);
   glMaterialfv(GL_FRONT, GL_DIFFUSE, brown);
   //glMaterialfv(GL_FRONT, GL_SPECULAR, white);

	/* move to location for object then draw it */
   for (int i = 0; i < model.faceCount; i++)
   {
      glBegin(GL_POLYGON);
      glNormal3f(model.faces[i].nx, model.faces[i].ny, model.faces[i].nz);
      for (int j = 0; j < model.faces[i].size; j++)
      {
         glVertex3f(model.vertexes[model.faces[i].vertexes[j]].x, model.vertexes[model.faces[i].vertexes[j]].y, model.vertexes[model.faces[i].vertexes[j]].z);
      }
      glEnd();
   }

   glPopMatrix ();
   glFlush ();
}

void reshape(int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   gluPerspective(45.0, (GLfloat)w/(GLfloat)h, 1.0, 10000000.0);
   glMatrixMode (GL_MODELVIEW);
   glLoadIdentity ();
}

void keyboard(unsigned char key, int x, int y) {
   switch (key) {
      case 27:
      case 'q':
         exit(0);
         break;
      case 'w':
         zoom += 1.0;
         break;
      case 's':
         zoom -= 1.0;
         break;
   }
   display();
}

void update() {
   rot += 0.001;
   display();
}

//read in model
void inportModel(FILE * fp)
{
   int hasVertexCount = 0;
   int hasFaceCount = 0;
   int readState = 0; //0 before end of header, 1 while reading vertexes, 2 while reading faces
   char line [128];
   int face = 0;
   int vertex = 0;

   while (fgets (line, 128, fp)!=NULL)
   {
      if (readState == 0)// reading header
      {
         char subject [64];
         if (sscanf(line, "%s", subject) != 1)
         {
            printf("Failed to read file.\n");
            fclose(fp);
            exit(1);
         }

         if (strcmp("element", subject) == 0)//read element
         {
            char element [64];
            if (sscanf(line, "%s %s", subject, element) != 2)
            {
               printf("Failed to read file.\n");
               fclose(fp);
               exit(1);
            }

            if (strcmp("vertex", element) == 0)//read in vertex count
            {
               if (hasVertexCount)
               {
                  printf("Failed to read file.\n");
                  fclose(fp);
                  exit(1);
               }
               int v = 0;
               if (sscanf(line, "element vertex %d", &v) != 1)
               {
                  printf("Failed to read file.\n");
                  fclose(fp);
                  exit(1);
               }

               //allocate vertexes
               model.vertexes = malloc(sizeof(Vertex) * v);
               model.vertexCount = v;
               hasVertexCount = 1;
            }
            else if (strcmp("face", element) == 0)//read in face count
            {
               if (hasFaceCount)
               {
                  printf("Failed to read file.\n");
                  fclose(fp);
                  exit(1);
               }
               int f = 0;
               if (sscanf(line, "element face %d", &f) != 1)
               {
                  printf("Failed to read file.\n");
                  fclose(fp);
                  exit(1);
               }

               //allocate faces
               model.faces = malloc(sizeof(Face) * f);
               model.faceCount = f;
               hasFaceCount = 1;
            }

         }
         else if (strcmp("end_header", subject) == 0)//move onto vertexes
         {
            if (!hasVertexCount || !hasFaceCount)
            {
               printf("Failed to read file.\n");
               fclose(fp);
               exit(1);
            }

            readState = 1;
         }
      }
      else if (readState == 1)//reading in vertexes
      {
         if (vertex < model.vertexCount)
         {
            //set values
            if (sscanf(line, "%f %f %f", &model.vertexes[vertex].x, &model.vertexes[vertex].y, &model.vertexes[vertex].z) != 3)
            {
               printf("Failed to read file.\n");
               fclose(fp);
               exit(1);
            }
            
            vertex++;
            if (vertex >= model.vertexCount)
            {
               readState = 2;
            }
         }
         else
         {
            readState = 2;
         }
         
      }
      else if (readState == 2)//read in faces
      {
         if (face < model.faceCount)
         {
            int v = 0;
            char * tok = strtok(line, " ");
            //set vertex count
            if (sscanf(tok, "%d", &v) != 1 || v < 3)
            {
               printf("Failed to read file.\n");
               fclose(fp);
               exit(1);
            }
            
            //allocate vertexes
            model.faces[face].size = v;
            model.faces[face].vertexes = malloc(sizeof (int) * v);

            //read in vertexes
            tok = strtok(NULL, " ");
            for (int i = 0; i < v && tok != NULL; i++)
            {
               int j;
               if (sscanf(tok, "%d", &j) != 1)
               {
                  printf("Failed to read file.\n");
                  fclose(fp);
                  exit(1);
               }
               model.faces[face].vertexes[i] = j;
               tok = strtok(NULL, " ");
            }

            //set normal
            float ux, uy, uz;
            float vx, vy, vz;

            ux = model.vertexes[model.faces[face].vertexes[1]].x - model.vertexes[model.faces[face].vertexes[0]].x;
            uy = model.vertexes[model.faces[face].vertexes[1]].y - model.vertexes[model.faces[face].vertexes[0]].y;
            uz = model.vertexes[model.faces[face].vertexes[1]].z - model.vertexes[model.faces[face].vertexes[0]].z;

            vx = model.vertexes[model.faces[face].vertexes[2]].x - model.vertexes[model.faces[face].vertexes[0]].x;
            vy = model.vertexes[model.faces[face].vertexes[2]].y - model.vertexes[model.faces[face].vertexes[0]].y;
            vz = model.vertexes[model.faces[face].vertexes[2]].z - model.vertexes[model.faces[face].vertexes[0]].z;

            model.faces[face].nx = (uy * vz) - (uz * vy);
            model.faces[face].ny = (uz * vx) - (ux * vz);
            model.faces[face].nz = (ux * vy) - (uz * vx);

            model.faces[face].nx *= -1.0;
            model.faces[face].ny *= -1.0;
            model.faces[face].nz *= -1.0;

            normalize(&model.faces[face].nx, &model.faces[face].ny, &model.faces[face].nz);

            face++;
         }
         else
         {
            printf("Failed to read file.\n");
            fclose(fp);
            exit(1);
         }
      }
   }
}


/*  Main Loop
 *  Open window with initial window size, title bar, 
 *  RGBA display mode, and handle input events.
 */
int main(int argc, char** argv) {

   if (argc != 2)
   {
      printf("No model given.\n");
      return 1;
   }

   FILE* fp = fopen(argv[1], "r");

   if (fp == NULL)
   {
      printf("Failed to open file: %s\n", argv[1]);
      return 1;
   }

   inportModel(fp);
   fclose(fp);

   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
   glutInitWindowSize (1440, 960);
   glutCreateWindow (argv[0]);
   init();
   glutReshapeFunc (reshape);
   glutDisplayFunc(display);
   glutKeyboardFunc (keyboard);
   glutIdleFunc(update);
   glutMainLoop();
   return 0; 
}

