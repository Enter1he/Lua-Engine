
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