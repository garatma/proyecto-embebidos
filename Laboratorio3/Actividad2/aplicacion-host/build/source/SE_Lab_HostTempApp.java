import processing.core.*; 
import processing.data.*; 
import processing.event.*; 
import processing.opengl.*; 

import processing.serial.*; 

import java.util.HashMap; 
import java.util.ArrayList; 
import java.io.File; 
import java.io.BufferedReader; 
import java.io.PrintWriter; 
import java.io.InputStream; 
import java.io.OutputStream; 
import java.io.IOException; 

public class SE_Lab_HostTempApp extends PApplet {


/**
 * Ejemplo de skecth Processing para el desarrollo del Laboratorio.
 *
 * Este skecth implementa un programa en el host que tiene la capacidad de
 * graficar funciones que evolucionan en el tiempo y además permite la
 * presentación de datos simples mediante Labels, y la captura de eventos
 * del usuario mediante la implementación de botones simples.
 *
 * La aplicación divide la ventana en 2 regiones, una de dibujado y otra 
 * donde se ubican los botones y etiquetas de información.
 *
 */


// Declaraciones para graficar funciones...
int cosVal;
int cantValues;
ScrollingFcnPlot factual, fmax, fmin, fprom;

Serial arduino;

int actual = 0,
	maximo = 0,
	minimo = 200,
	prom = 0,
	suma = 0,
	indice = 0,
	cantidad_censados = 0,
	modo = 1,
	cuenta = 0;

int [] muestras = new int[60];

//Botones...
RectButton boton_actual, boton_max, boton_min, boton_prom;
PFont myFont;

//Etiquetas textuales...
boolean alert = false;
boolean prealert = false;
Label label_texto_actual, label_temperatura_actual, 
	  label_texto_max, label_temperatura_max, 
	  label_texto_min, label_temperatura_min, 
	  label_texto_prom, label_temperatura_prom; 
Label yVal1,yVal2,yVal3,yVal4,yVal5,yVal6,yVal7,yVal8;

//Ventana y viewports...
int pgFunctionViewportWidth = 480;
int pgControlViewportWidth = 120;
int pgViewportsHeight = 240;

public void setup() 
{

  	//Se define el tamaño de la ventana de la aplicación... 
  	
  	
  	//Se inicializan los arreglos para almacenar las funciones...
  	cantValues = pgFunctionViewportWidth;
  	factual = new ScrollingFcnPlot(60,color(0,100,0),0,30);
  	fmax = 	new ScrollingFcnPlot(60,color(100,0,0),0,30);
  	fmin = 	new ScrollingFcnPlot(60,color(0,0,100),0,30); 
  	fprom = 	new ScrollingFcnPlot(60,color(100,100,0),0,30);

  	//Se inicializan los botones...
  	boton_actual = new RectButton(pgFunctionViewportWidth+10,10,60,40,color(102),color(50),color(255),"Actual");
  	boton_max = new	RectButton(pgFunctionViewportWidth+10,60,60,40,color(102),color(50),color(255),"Max");
   	boton_min = new	RectButton(pgFunctionViewportWidth+100,10,60,40,color(102),color(50),color(255),"Min"); 	
  	boton_prom = new RectButton(pgFunctionViewportWidth+100,60,60,40,color(102),color(50),color(255),"Prom");

	//Se inicializan los labels...
  	label_texto_actual = new Label(pgFunctionViewportWidth+10,130,color(0,100,0),"T.Actual: ");
  	label_temperatura_actual = new Label(pgFunctionViewportWidth+75,130,color(0,100,0),"-");
  	label_texto_max = new Label(pgFunctionViewportWidth+10,150,color(100,0,0),"T.Max: ");
  	label_temperatura_max = new	Label(pgFunctionViewportWidth+75,150,color(100,0,0),"-");
  	label_texto_min = new Label(pgFunctionViewportWidth+10,170,color(0,0,100),"T.Min: ");
  	label_temperatura_min = new Label(pgFunctionViewportWidth+75,170,color(0,0,100),"-");
  	label_texto_prom = new Label(pgFunctionViewportWidth+10,190,color(100,100,0),"T.Prom: ");
  	label_temperatura_prom = new Label(pgFunctionViewportWidth+75,190,color(100,100,0),"-");
  	
  	yVal1 = new Label(10,5,color(255),"30");
  	yVal2 = new Label(10,37,color(255),"25");
  	yVal3 = new Label(10,220,color(255),"0");
    yVal4 = new Label(10,73,color(255),"20");
    yVal5 = new Label(10,110,color(255),"15");
    yVal6 = new Label(10,147,color(255),"10");
    yVal7 = new Label(10,183,color(255),"5");
  	//Inicializa el font de la GUI...

	arduino = new Serial(this,Serial.list()[0],9600);
	arduino.buffer(2);
}

public void draw() 
{
  	//Se actualizan las funciones de ejemplo (f1: coseno(x), f2: función que depende de la posición Y del mouse)
  	
  	factual.updateValue(actual);
  	fmax.updateValue(maximo);
  	fmin.updateValue(minimo);
  	fprom.updateValue(prom);
  	
  	  	
  	
  	//Permite expermientar con la velocidad de scroll al actualizar más lentamente los valores...
  	//delay(50);
  	   
  	//Rendering de la interface...
  	background(125);
  	stroke(0);
  	noFill();
 
  	//Dibuja las funciones...
  	factual.displayIntoRect(30,10,pgFunctionViewportWidth-10,pgViewportsHeight-10);
  	fmax.displayIntoRect(30,10,pgFunctionViewportWidth-10,pgViewportsHeight-10);
  	fmin.displayIntoRect(30,10,pgFunctionViewportWidth-10,pgViewportsHeight-10);
  	fprom.displayIntoRect(30,10,pgFunctionViewportWidth-10,pgViewportsHeight-10);

  	//Procesa eventos de MouseOver...
  	boton_actual.update();
  	boton_max.update();
  	boton_min.update();
  	boton_prom.update();

	//Procesa las entradas (botones)
  	if(mousePressed) 
	{
  	  	if(boton_actual.pressed()) 
		{
			arduino.write('1');
			boton_actual.currentcolor = color(0,100,0);
		}
  	  	else if(boton_max.pressed()) 
		{
			arduino.write('2');
			boton_max.currentcolor = color(0,100,0);
		}
  		else if(boton_min.pressed())
		{
			arduino.write('3');
			boton_min.currentcolor = color(0,100,0);
		}
  		else if(boton_prom.pressed())
		{
			arduino.write('4');
			boton_prom.currentcolor = color(0,100,0);
		}
  	}

  	
  	//Dibuja el eje X y el recuadro de los gráficos...
  	stroke(0);
  	
  
  	rect(30,10,pgFunctionViewportWidth-40,pgViewportsHeight-20);
  	
  	//Se dibujan los botones...
  	boton_actual.display();
  	boton_max.display();
  	boton_min.display();
  	boton_prom.display();
  	
  	//Se dibuja texto adicional...(labels, etc)
  	label_texto_actual.display();
  	label_temperatura_actual.display();
  	label_texto_max.display();
  	label_temperatura_max.display();
  	label_texto_min.display();
  	label_temperatura_min.display();
  	label_texto_prom.display();
  	label_temperatura_prom.display();

  	yVal1.display();
  	yVal2.display();
  	yVal3.display();  
    yVal4.display();  
    yVal5.display();  
    yVal6.display();  
    yVal7.display();  
}

public void serialEvent(Serial p)
{
	String actual_string = p.readString();
	if ( cuenta > 5 )
	{
		actual = PApplet.parseInt(actual_string);
		if ( actual < minimo ) minimo = actual;
		else if ( actual > maximo ) maximo = actual;
		
		if ( cantidad_censados < 60 )
			cantidad_censados++;
		else
			suma -= muestras[indice];

		muestras[indice] = actual;
		suma += actual;
		prom = (int) suma/cantidad_censados;

		indice++;
		if ( indice == 60 )
			indice = 0;

		label_temperatura_actual.setCaption(str(actual));
		label_temperatura_max.setCaption(str(maximo));
		label_temperatura_min.setCaption(str(minimo));
		label_temperatura_prom.setCaption(str(prom));
	}
	else
		cuenta++;
}
/**
 * Clases que definen botones.
 * Se basan en el ejemplo Topics > GUI > Buttons de Processing 1.5.
 *
 * http://processing.org/learning/topics/buttons.html
 *
 */

class Button
{
  int x, y;
  int size, size2;
  int basecolor, highlightcolor;
  int currentcolor,textcolor;
  boolean over = false;
  boolean pressed = false;   
  boolean locked = false;
  String caption = "Button";

  public void update() 
  {
    if(over()) {
      currentcolor = highlightcolor;
    } 
    else {
      currentcolor = basecolor;
    }
  }

  public boolean pressed() 
  {
    if(over) {
      locked = true;
      return true;
    } 
    else {
      locked = false;
      return false;
    }    
  }

  public boolean over() 
  { 
    return true; 
  }

  public boolean overRect(int x, int y, int width, int height) 
  {
    if (mouseX >= x && mouseX <= x+width && 
      mouseY >= y && mouseY <= y+height) {
      return true;
    } 
    else {
      return false;
    }
  }

  public boolean overCircle(int x, int y, int diameter) 
  {
    float disX = x - mouseX;
    float disY = y - mouseY;
    if(sqrt(sq(disX) + sq(disY)) < diameter/2 ) {
      return true;
    } 
    else {
      return false;
    }
  }

}

class CircleButton extends Button
{ 
  CircleButton(int ix, int iy, int isize, int icolor, int ihighlight, int textcolor, String caption) 
  {
    x = ix;
    y = iy;
    size = isize;
    basecolor = icolor;
    highlightcolor = ihighlight;
    currentcolor = basecolor;
    this.textcolor = textcolor;
    this.caption = caption;
  }

  public boolean over() 
  {
    if( overCircle(x, y, size) ) {
      over = true;
      return true;
    } 
    else {
      over = false;
      return false;
    }
  }

  public void display() 
  {
    stroke(255);
    fill(currentcolor);
    ellipse(x, y, size, size);
    fill(textcolor);
    textAlign(CENTER,CENTER);
    text(caption, x+size/2, y+size/2);
  }
}

class RectButton extends Button
{
  RectButton(int ix, int iy, int isize, int isize2, int icolor, int ihighlight, int textcolor, String caption) 
  {
    x = ix;
    y = iy;
    size = isize;
    size2 = isize2;
    basecolor = icolor;
    highlightcolor = ihighlight;
    currentcolor = basecolor;
    this.textcolor = textcolor;
    this.caption = caption;
  }

  public boolean over() 
  {
    if( overRect(x, y, size, size2) ) {
      over = true;
      return true;
    } 
    else {
      over = false;
      return false;
    }
  }

  public void display() 
  {
    stroke(255);
    fill(currentcolor);
    rect(x, y, size, size2);
    fill(textcolor);
    textAlign(CENTER,CENTER);
    text(caption, x+size/2, y+size2/2);
  }
}
/**
 * Clase que define etiquetas textuales para la GUI de la aplicación.
 * Define una posición (esquina superior izquierda) para la etiqueta, el
 * contenido de la misma y el color para el renderizado del texto.

 */

class Label
{
  int x, y;
  int textcolor;
  String caption;

  Label(int x, int y, int textcolor, String caption)
  {
    this.x = x;
    this.y = y;
    this.textcolor = textcolor;
    this.caption = caption;
  }
  
  public void display() 
  {
    stroke(255);
    fill(textcolor);
    textAlign(LEFT,TOP);
    text(caption, x, y);
  }  

  public void setCaption(String caption)
  {
	  this.caption = caption;
  }
}
/**
 * Clase que define un graficador de funciones que hace scroll horizontal en el tiempo.
 * Permite definir un conjunto arbitrario de funciones y plotearlas en el mismo gráfico.
 * El scroll del gráfico es controlado por las modificaciones a los valores del mismo (si no
 * se producen actualizaciones en los valores de las funciones, el gráfico no se desplaza).

 */
 
class ScrollingFcnPlot
{
  float[] f;
  int colorF;
  int fPtr;
  int cantValues;
  boolean mustScroll = false;
  int maxVal, minVal;
  
  ScrollingFcnPlot (int cantValues, int colorF, int minVal, int maxVal)
  {
    this.f = new float[cantValues];
    this.fPtr = 0;
    this.cantValues = cantValues;
    this.colorF = colorF;
    this.maxVal = maxVal;
    this.minVal = minVal;    
  }
  
  public void updateValue(float value)
  {
    this.f[fPtr] = value;
    this.mustScroll = true;
      
    //Se incrementa el puntero circular
    fPtr = (fPtr + 1) % cantValues;
  }
  
  public void displayIntoRect(int x1,int y1,int x2,int y2)
  {
    if (mustScroll)
    {
      mustScroll = false;
      
      stroke(colorF);
      
      beginShape();
      for(int i = 0; i<f.length;i++){
        vertex(map(i,0,cantValues,x1,x2),map(f[(fPtr+i)%cantValues], minVal, maxVal, y2, y1));
      }
      endShape();
    }  
  }
}
  public void settings() { 	size(660, 240); }
  static public void main(String[] passedArgs) {
    String[] appletArgs = new String[] { "SE_Lab_HostTempApp" };
    if (passedArgs != null) {
      PApplet.main(concat(appletArgs, passedArgs));
    } else {
      PApplet.main(appletArgs);
    }
  }
}
