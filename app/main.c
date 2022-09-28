#include <GL/glew.h>

//Przed uruchomieniem nalezy w Shell zainstalowac biblioteke:
//install-pkg -y libglew-dev

//Uruchomienie po wpisaniu:
//make && ./bin/demo

/*
Przykladowe poprawne wyrazenia:
2.5-2;
5.1^2-(4+2);
(100-4^2)+1;5.6*1;
*/

/*
Przykladowe niepoprawne wyrazenia:
2(2.5);
--2(100);
+1)2;
*/

#include <stdio.h>
#include <string.h>
 
#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT
#include "nuklear.h"
#include "nuklear_glfw_gl3.h"
 
 
static void error_callback(int e, const char *d)
{printf("Error %d: %s\n", e, d);}
 
 
/*
Poprawiona gramatyka
 
S ::= W ; Z
Z ::= W ; Z | eps
W ::= PW’
W’ ::= OW | eps
P ::= R | (W)
R ::= LR’
R’ ::= .L | eps
L ::= CL’
L’ ::= L | eps
C ::= 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9
O ::= *| : | + | - | ˆ
*/
 
//Zbiory symboli pierwszych
int FirstS [] = {'0','1','2','3','4','5','6','7','8','9','('};
int FirstZ [] = {'0','1','2','3','4','5','6','7','8','9','('};
int FirstW [] = {'0','1','2','3','4','5','6','7','8','9','('};
int FirstWprim [] = {'*',':','+','-','^'};
int FirstP [] = {'0','1','2','3','4','5','6','7','8','9','('};
int FirstR [] = {'0','1','2','3','4','5','6','7','8','9'};
int FirstRprim [] = {'.'};
int FirstL [] = {'0','1','2','3','4','5','6','7','8','9'};
int FirstLprim [] = {'0','1','2','3','4','5','6','7','8','9'};
int FirstC [] = {'0','1','2','3','4','5','6','7','8','9'};
int FirstO [] = {'*',':','+','-','^'};
 
//Tablica do przechowywania ilosci elementow w danych zbiorach symboli pierwszych
int n [] = {11,11,11,5,11,10,1,10,10,10,5};
 
char w[256]; //tablica do przechowywania wyrazenia
char wtemp[256] = {0}; //pomocnicza tablica do przechowywania wyrazenia
int wId = -1; //zmienna przechowujaca pozycje analizowanego symbolu z wyrazenia w
int maxW = 0; //dlugosc wyrazenia w
 
//Funkcje badajace zgodnosc z produkcjami O,C,L',L,R',R,P,W',W,Z,S. Zwracaja 1 w przypadku zgodnosci z produkcja.
int productionO();
int productionC();
int productionLprim();
int productionL();
int productionRprim();
int productionR();
int productionP();
int productionWprim();
int productionW();
int productionZ();
int productionS();
 
//Funkcja sprawdzajaca czy dany symbol znajduje sie w zbiorze symboli pierwszych FirstN, ktory posiada n-elementow. Jezeli symbol zawiera sie w FirstN, to funkcja zwraca 1, a przeciwnym razie 0
int isInFirst(int FirstN[], int n, char symbol[]){
  if(symbol == NULL)
    return 0;
  for(int i=0;i<n;i++){
    if(FirstN[i] == symbol[0])
      return 1;
  }
  return 0;
}
 
//Funkcja zwiekszajaca indeks wId, a nastepnie sprawdzająca czy dany symbol c wystepuje w tablicy w na pozycji wId. Jezeli wystepuje, to funkcja zwraca 1, a w przeciwnym razie 0.
int isCharInW(char c){
  wId++;
  if(wId < maxW){
    if(w[wId] == c)
      return 1;
  }
  return 0;
}
 
//Funkcja zwiększajaca wId o 1 oraz zwracajaca wskaznik do symbolu znajdujacego się w tablicy w pod indeksem wId. Jezeli wId+1 jest mniejsze od maxW, to funkcja zwraca NULL
char *readNext(char w[]){
  if(wId+1 < maxW){
    wId += 1;
    //printf("READ %c\n", w[wId]);
    return &w[wId];
  }
  return NULL;
}
 
//Funkcja dla produkcji O ::= *| : | + | - | ˆ, ktora w przypadku zgodnosci zwraca 1
int productionO(){
  wId++;
  if(wId >= maxW)
    return 0;
 
//sprawdzenie czy aktualnie analizowany symbol w[wId] nalezy do zbioru symboli pierwszych O, ktory liczy n[10] elementow
  if(isInFirst(FirstO, n[10], &w[wId]))
    return 1;
  return 0;
}
 
//Funkcja dla produkcji C ::= 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9, ktora w przypadku zgodnosci zwraca 1
int productionC(){
  wId++;
  if(wId >= maxW)
    return 0;
 
//sprawdzenie czy aktualnie analizowany symbol w[wId] nalezy do zbioru symboli pierwszych C, ktory liczy n[9] elementow
  if(isInFirst(FirstC, n[9], &w[wId]))
    return 1;
  return 0;
}
 
//Funkcja dla produkcji L’ ::= L | eps, ktora w przypadku zgodnosci zwraca 1
int productionLprim(){
  int temp = wId;
  if(!productionL())
    wId = temp;
  return 1;
}
 
//Funkcja dla produkcji L ::= CL’, ktora w przypadku zgodnosci zwraca 1
int productionL(){
  return productionC() && productionLprim();
}
 
//Funkcja dla produkcji R’ ::= .L | eps, ktora w przypadku zgodnosci zwraca 1
int productionRprim(){
  int temp = wId;
  if(isCharInW('.')){
    return productionL();
  }else{
    wId = temp;
    return 1;
  }
  return 0;
}
 
//Funkcja dla produkcji R ::= LR’, ktora w przypadku zgodnosci zwraca 1
int productionR(){
  return productionL() && productionRprim();
}
 
//Funkcja dla produkcji P ::= R | (W), ktora w przypadku zgodnosci zwraca 1
int productionP(){
  int temp = wId;
  if(!productionR()){
    wId = temp;
    return isCharInW('(') && productionW() && isCharInW(')');
  }else{
    return 1;
  }
  return 0;
}
 
//Funkcja dla produkcji W’ ::= OW | eps, ktora w przypadku zgodnosci zwraca 1
int productionWprim(){
  int temp = wId;
  if(productionO()){
    return productionW();
  }else{
    wId = temp;
    return 1;
  }
}
 
//Funkcja dla produkcji W ::= PW’, ktora w przypadku zgodnosci zwraca 1
int productionW(){
  return productionP() && productionWprim();
}
 
//Funkcja dla produkcji Z ::= W ; Z | eps, ktora w przypadku zgodnosci zwraca 1.
int productionZ(){
  int temp = wId;
  if(productionW()){
    return isCharInW(';') && productionZ();
  }else{
    wId = temp;
    return 1;
  }
}
//Funkcja dla produkcji S ::= W ; Z, ktora zwraca 1 w przypadku zgodnosci
int productionS(){
  if(productionW() && isCharInW(';') && productionZ()){
    if(wId == maxW - 1)
      return 1;
  }
  return 0;
}
 
//Funkcja czyszczaca ze spacji wprowadzone do tablicy wtemp wyrazenie, a nastepnie zapisanie rezultatu do tablicy w
void clearWhiteSpace(){
  int j = 0;
  for(int i=0; i<maxW; i++){
    if(wtemp[i]!= ' '){
      w[j]=wtemp[i];
      j++;
    }
  }
  w[j] = '\0';
}

int main(void)
{
  int win_width = 400;
  int win_height = 300;
 
  struct nk_glfw glfw = {0};
  static GLFWwindow *win;
  glfwSetErrorCallback(error_callback);

  if(!glfwInit()){
      fprintf(stdout, "[GFLW] failed to init!\n");
      exit(1);
  }
 
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  #endif

  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  win = glfwCreateWindow(win_width, win_height, "Analizator LL(1)", NULL, NULL);
  glfwMakeContextCurrent(win);
 
  glewExperimental = 1;
  if(glewInit() != GLEW_OK){
    fprintf(stderr, "Failed to setup GLEW\n");
    exit(1);
  }
 
	/* Utworzenie kontekstu */
  struct nk_context *ctx = nk_glfw3_init(&glfw, win, NK_GLFW3_INSTALL_CALLBACKS);
 
  {
  struct nk_font_atlas *atlas;
  nk_glfw3_font_stash_begin(&glfw, &atlas);
  nk_glfw3_font_stash_end(&glfw);
  }

  int option = 0;
  while(!glfwWindowShouldClose(win)){
      glfwPollEvents();
      nk_glfw3_new_frame(&glfw);
 
      /* GUI */
      if(nk_begin(ctx, "Okno glowne", nk_rect(0, 0, win_width, win_height), NK_WINDOW_BORDER)){
          nk_layout_row_dynamic(ctx, 120, 1);
          nk_label(ctx, "Wprowadz wyrazenie:", NK_TEXT_CENTERED);
	        nk_layout_row_dynamic(ctx, 30, 1);
          nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, wtemp, sizeof(wtemp) -1, nk_filter_default);
	        nk_layout_row_static(ctx, 30, 80, 1);

	        if(nk_button_label(ctx, "Sprawdz")){
		        maxW = strlen(wtemp);
		        clearWhiteSpace();
		        maxW = strlen(w);
		           if(productionS())
   		         option = 1;
	             else
		           option = 2;
		       }
      }
      nk_end(ctx);
 
      if(option==1){
        if(nk_begin(ctx, "Okno_1", nk_rect(0, 0, win_width, win_height), 0)){
            nk_layout_row_dynamic(ctx, 120, 1);
            nk_label(ctx, "Wyrazenie jest zgodne z gramatyka", NK_TEXT_CENTERED);
 
	          nk_layout_row_dynamic(ctx, 30, 1);
            nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, wtemp, sizeof(wtemp) -1, nk_filter_default);
            nk_layout_row_static(ctx, 30, 80, 1);
 
            nk_layout_row_static(ctx, 30, 80, 1);
	          if(nk_button_label(ctx, "Wyczysc")){
		           option = 0;
		           memset(wtemp,0,255);
		        }
         }
         nk_end(ctx);
      }
 
      if(option==2){
        if(nk_begin(ctx, "Okno_2", nk_rect(0, 0, win_width, win_height), 0)){
            nk_layout_row_dynamic(ctx, 120, 1);
            nk_label(ctx, "Wyrazenie jest niezgodne z gramatyka", NK_TEXT_CENTERED);
 
	          nk_layout_row_dynamic(ctx, 30, 1);
            nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, wtemp, sizeof(wtemp) -1, nk_filter_default);
            nk_layout_row_static(ctx, 30, 80, 1);
 
            nk_layout_row_static(ctx, 30, 80, 1);
	          if(nk_button_label(ctx, "Wyczysc")){
		           option = 0;
		           memset(wtemp,0,255);
		        }
         }
         nk_end(ctx);
      }
    glViewport(0, 0, win_width, win_height);
    glClear(GL_COLOR_BUFFER_BIT);
    nk_glfw3_render(&glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
    glfwSwapBuffers(win);
  }
  nk_glfw3_shutdown(&glfw);
  glfwTerminate();
  return 0;
}