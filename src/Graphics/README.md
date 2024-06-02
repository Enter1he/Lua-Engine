# Graphics

This is official readme reference for Graphics module of LuaEngine,
all functions available in the module explained here.

## Primitives

LuaEngine allows drawing simple primitives like rectangles and circles.

```
Graphics.SetColor( r,g,b,a : number)-> nil
```
Sets color for drawing primitives only affects them.


```
Graphics.DrawLine( x1,y1,x2,y2 : number )-> nil
```
Draws line from position {x1,y1} to {x2, y2}


```
Graphics.DrawLines(arr : table(array[1..n] : number))-> nil
```
Draws one combination of lines using table array in form {x1,y1, ... xn, yn}


```
Graphics.DrawRect(lined : boolean; x,y,w,h : number) -> nil
```
Draws a rectangle on position {x,y} with width and height corresponding to values 'w','h'
Depending on the value 'lined' object would appear wireframed or filled with current color.

```
Graphics.DrawQuad(lined : boolean; x1,y1, x2,y2, x3,y3, x4,y4 : number) -> nil
```
Draws a quad each point of which is explained with values xn, yn.


```
Graphics.DrawCircle(lined : boolean; x,y,r : number) -> nil
```
Draws a circle with centre at position {x,y} and radius corresponding to value 'r'.
Depending on the value 'lined' object would appear wireframed or filled with current color.

