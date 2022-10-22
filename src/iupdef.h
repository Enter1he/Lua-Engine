#ifndef IUPDEF_H
#define IUPDEF_H

#define IUP_API __declspec(dllexport)
typedef struct Ihandle_ Ihandle;
typedef int (*Icallback)(Ihandle*);
IUP_API void IupGLCanvasOpen(void);
IUP_API Ihandle *IupGLCanvas(const char* action);
IUP_API void IupGLMakeCurrent(Ihandle *ih);
IUP_API void IupGLSwapBuffers(Ihandle *ih);
IUP_API Ihandle *IupSetFocus(Ihandle *ih);
IUP_API int IupOpen(int *argc, char*** argv);
IUP_API void IupClose(void);
IUP_API Ihandle*  IupDialog     (Ihandle* child);
IUP_API Ihandle*  IupZbox       (Ihandle* child, ...);
IUP_API Ihandle* IupSetHandle(const char *name, Ihandle* ih);
IUP_API void IupSetAttribute(Ihandle* ih, const char* name, const char* value);
IUP_API Icallback IupSetFunction(const char *name, Icallback func);
IUP_API Icallback IupSetCallback (Ihandle* ih, const char *name, Icallback func);
IUP_API Ihandle* IupSetCallbacks(Ihandle* ih, const char *name, Icallback func, ...);
IUP_API Ihandle* IupTimer(void);
IUP_API void IupSetInt(Ihandle* ih, const char* name, int value);
IUP_API int IupShow(Ihandle* ih);
IUP_API int IupLoopStep(void);
IUP_API void IupRedraw(Ihandle* ih, int children);
IUP_API void IupUpdate(Ihandle *ih);
IUP_API void IupDestroy(Ihandle* ih);
void IupMessageError(Ihandle* parent, const char *message);

#define IUP_BUTTON1   '1'
#define IUP_BUTTON2   '2'
#define IUP_BUTTON3   '3'
#define IUP_BUTTON4   '4'
#define IUP_BUTTON5   '5'

#define iup_isshift(_s)    (_s[0]=='S')
#define iup_iscontrol(_s)  (_s[1]=='C')
#define iup_isbutton1(_s)  (_s[2]=='1')
#define iup_isbutton2(_s)  (_s[3]=='2')
#define iup_isbutton3(_s)  (_s[4]=='3')
#define iup_isdouble(_s)   (_s[5]=='D')
#define iup_isalt(_s)      (_s[6]=='A')
#define iup_issys(_s)      (_s[7]=='Y')
#define iup_isbutton4(_s)  (_s[8]=='4')
#define iup_isbutton5(_s)  (_s[9]=='5')

#endif