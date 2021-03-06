
#include "PeepLib.h"

const int KEYROWCOUNT=5;
const int KEYCOLCOUNT=10;

void paintButton(Adafruit_GFX* tft,int x,int y,int width,int height,String text,int textSize){
	tft->fillRect(x,y,width,height,RISEDSURFACE);
	tft->drawLine(x,y,x+width,y,SHADELIGHT);
	tft->drawLine(x,y,x,y+height,SHADELIGHT);
	tft->drawLine(x,y+height,x+width,y+height,SHADEDARK);
	tft->drawLine(x+width,y,x+width,y+height,SHADEDARK);
	tft->setTextColor(TEXTCOLOR, GRAY),
	tft->setTextSize(textSize);
	tft->setCursor(x+10,y+height/2-5);
	tft->println(text);  	
	
}

char KEYBOARD[KEYROWCOUNT][KEYCOLCOUNT]= {{'1','2','3','4','5','6','7','8','9','0'},{'Q','W','E','R','T','Z','U','I','O','P'},
{'A','S','D','F','G','H','J','K','L',(char)13},
{'Y','X','C','V','B','N','M','(',')',(char)8},
{'<','>',(char)12,';',':',',','-',(char)32,'@','?'}};

int closeButtonSize=30;


void Paintable::setZIndex(int zIndex){
	this->zIndex=zIndex;
}

int Paintable::getZIndex(){
	return zIndex;
}

Peep::Peep(Adafruit_GFX *tft):Paintable(){
	setZIndex(0);
	this->tft=tft;
	tft->fillScreen(BACKGROUND); 
 	elementCount=-1;
	uint16_t g_identifier;
	keyboardVisible=false;
 
}



void Peep::repaint(){
	tft->fillScreen(BACKGROUND); 
	if(elementCount>-1){
		for(int i=0;i<elementCount;i++){
			controlElements[i]->paintElement();	
		}
	}
}

Adafruit_GFX* Peep::getDisplay(){
	return tft;
}


void Peep::addElement(ControlElement *controlElement){
	if(elementCount==-1){
		elementCount=0;
	}
	controlElement->setPaintable(this);
	
	this->controlElements[elementCount]=controlElement;
	
	elementCount++;

	controlElement->paintElement();
}



Event Peep::handleTouch(int x,int y){
	if(keyboardVisible){
		if(x>=getDisplay()->width()-closeButtonSize && y<=closeButtonSize){
			hideKeyboard();
			Event evt;
			evt.id="Peep";
			evt.eventType=EVT_KEYBOARDHIDE;
			return evt;
		}else{
			
			if(y>getDisplay()->height()-KEYROWCOUNT*keyButtonHeight){
				int row=KEYROWCOUNT-1-(int)((((float)((-1)*(y-getDisplay()->height())))/((float)(KEYROWCOUNT*keyButtonHeight)))*(float)KEYROWCOUNT);
				int col=(int)((((float)(x))/((float)(KEYCOLCOUNT*keyButtonHeight)))*(float)KEYCOLCOUNT);
				if((int)KEYBOARD[row][col]==8){
					keyboardText=keyboardText.substring(0,keyboardText.length()-1);
				}else
				if((int)KEYBOARD[row][col]==12){
					shift=!shift;
					refreshKeyboard();
				}else
				if((int)KEYBOARD[row][col]==13){
					hideKeyboard();
					Event evt;
					evt.id="Peep";
					evt.eventType=EVT_KEYBOARDSHOW;
					return evt;
				}else
				{
					if(shift){
						keyboardText=keyboardText+String(KEYBOARD[row][col]);
					}else{
						keyboardText=keyboardText+String((char)((int)KEYBOARD[row][col]+32));
					}
					
				}
				getDisplay()->fillRect(10,closeButtonSize+10,getDisplay()->width()-20,30,WHITE);
				getDisplay()->drawRect(10,closeButtonSize+10,getDisplay()->width()-20,30,BLACK);
				getDisplay()->setTextColor(TEXTCOLOR, WHITE),
				getDisplay()->setTextSize(2);
				getDisplay()->setCursor(15,closeButtonSize+15);
				getDisplay()->println(keyboardText); 
				delay(200);				
			}
			Event evt;
			evt.id="Peep";
			evt.eventType=EVT_KEYBOARDSHOW;
			return evt;
		}
	}else{
		if(elementCount>-1){
			for(int i=0;i<elementCount;i++){
				if(controlElements[i]->getZIndex()>=this->getZIndex()){	
					int retVal=controlElements[i]->handleTouch(x,y);
				
					if(retVal>0){
						if(controlElements[i]->requestsInput()){
							controlEditing=controlElements[i];
							showKeyboard();
							Event evt;
							evt.id="Peep";
							evt.eventType=EVT_KEYBOARDSHOW;
							return evt;
						}else{
							Event evt;
							evt.id=controlElements[i]->getID();
							evt.eventType=EVT_CLICK;
							evt.customType=retVal;
							return evt;
						}
					}
				}
			}
		}
		Event evt;
		evt.id="Peep";
		evt.eventType=EVT_NONE;
		return evt;
		
	}
	
}

void Peep::refreshKeyboard(){
int keyCount=sizeof(KEYBOARD[0])/sizeof(KEYBOARD[0][0]);

	keyButtonHeight=getDisplay()->width()/keyCount;
	getDisplay()->setTextColor(TEXTCOLOR, WHITE),
	getDisplay()->setTextSize(1);
	for(int k=0;k<KEYROWCOUNT;k++){ 
		for(int i=0;i<keyCount;i++){
			getDisplay()->fillRect(i*keyButtonHeight,getDisplay()->height()-(KEYROWCOUNT-k)*keyButtonHeight,keyButtonHeight,keyButtonHeight,WHITE);
			getDisplay()->drawRect(i*keyButtonHeight,getDisplay()->height()-(KEYROWCOUNT-k)*keyButtonHeight,keyButtonHeight,keyButtonHeight,BLACK);
			getDisplay()->setCursor(i*keyButtonHeight+5,getDisplay()->height()-(KEYROWCOUNT-k)*keyButtonHeight+5);
			if((int)KEYBOARD[k][i]==13){
				getDisplay()->println("ENT");
			}
			else
			if((int)KEYBOARD[k][i]==8){
				getDisplay()->println("<-");
			}
			else
			if((int)KEYBOARD[k][i]==32){
				getDisplay()->println("SPC");
			}
			else
			
			if((int)KEYBOARD[k][i]==12){
				getDisplay()->println("SHFT");
			}
			else{
				if(!shift && (int)KEYBOARD[k][i]>=65 && (int)KEYBOARD[k][i]<=90){
					getDisplay()->println(String((char)((int)KEYBOARD[k][i]+32)));
				}else{
					getDisplay()->println(String(KEYBOARD[k][i]));
				}
				
				
			}
					
			 
			
		}
	}
}

void Peep::showKeyboard(){
	if(controlEditing){
		keyboardText=controlEditing->getText();
	}else{
		keyboardText="";
	}
	shift=false;
	refreshKeyboard();
	
	getDisplay()->fillRect(getDisplay()->width()-closeButtonSize,0,closeButtonSize,closeButtonSize,WHITE);
	getDisplay()->drawRect(getDisplay()->width()-closeButtonSize,0,closeButtonSize,closeButtonSize,BLACK);
	getDisplay()->drawLine(getDisplay()->width()-closeButtonSize,0,getDisplay()->width(),closeButtonSize,BLACK);
	getDisplay()->drawLine(getDisplay()->width()-closeButtonSize,closeButtonSize,getDisplay()->width(),0,BLACK);

	
	getDisplay()->fillRect(10,closeButtonSize+10,getDisplay()->width()-20,30,WHITE);
	getDisplay()->drawRect(10,closeButtonSize+10,getDisplay()->width()-20,30,BLACK);
	getDisplay()->setTextColor(TEXTCOLOR, WHITE);
	getDisplay()->setTextSize(2);
	getDisplay()->setCursor(15,closeButtonSize+15);
	getDisplay()->println(keyboardText); 
	
	keyboardVisible=true;
	
}

void Peep::hideKeyboard(){
	keyboardVisible=false;
	repaint();
	if(controlEditing){
		controlEditing->setText(keyboardText);
	}
}



ControlElement::ControlElement(String id,int x,int y,int width,int height){
	this->x=x;
	this->y=y;
	this->width=width;
	this->height=height;
	this->id=id;
	this->zIndex=0;
}

void ControlElement::setPaintable(Paintable *paintable){
	this->paintable=paintable;
}


int ControlElement::getZIndex(){
	return zIndex;
}

String ControlElement::getID(){
	return this->id;
}

void ControlElement::paintElement(){

}


int ControlElement::getX(){
	return this->x;
}
int ControlElement::getY(){
	return this->y;
}
int ControlElement::getWidth(){
	return this->width;
}
int ControlElement::getHeight(){
	return this->height;
}

bool ControlElement::requestsInput(){
	return false;
}

void ControlElement::setText(String text){}

String ControlElement::getText(){
	return "";
}

Button::Button(String id,int x,int y,int width,int height,String text):ControlElement(id,x,y,width,height){
	this->text=text;
}

void Button::paintElement(){


	paintButton(paintable->getDisplay(),this->getX(),this->getY(),this->getWidth(),this->getHeight(),text,1);
	
}

int Button::handleTouch(int x,int y){
	if(this->x<=x && this->x+this->width>=x && y>=this->y && y<=this->y+this->height){
		return 1;
	}else{
		return -1;
	}
}

Checkbox::Checkbox(String id,int x,int y,String text):ControlElement(id,x,y,30,30){
	this->text=text;
}

void Checkbox::paintElement(){

 	paintable->getDisplay()->fillRect(this->getX(),this->getY(),this->getWidth(),this->getHeight(),GRAY);
	paintable->getDisplay()->drawLine(this->getX(),this->getY(),this->getX()+this->getWidth(),this->getY(),WHITE);
	paintable->getDisplay()->drawLine(this->getX(),this->getY(),this->getX(),this->getY()+this->getHeight(),WHITE);
	paintable->getDisplay()->drawLine(this->getX(),this->getY()+this->getHeight(),this->getX()+this->getWidth(),this->getY()+this->getHeight(),DARKGRAY);
	paintable->getDisplay()->drawLine(this->getX()+this->getWidth(),this->getY(),this->getX()+this->getWidth(),this->getY()+this->getHeight(),DARKGRAY);
	if(isChecked()){
		paintable->getDisplay()->drawLine(this->getX()+2,this->getY()+2,this->getX()+this->getWidth()-2,this->getY()+this->getHeight()-2,DARKGRAY);
		paintable->getDisplay()->drawLine(this->getX()+this->getWidth()-2,this->getY()+2,this->getX()+2,this->getY()+this->getHeight()-2,DARKGRAY);	
	}
	paintable->getDisplay()->setTextColor(TEXTCOLOR, BACKGROUND),
	paintable->getDisplay()->setTextSize(2);
	paintable->getDisplay()->setCursor(this->getX()+40,this->getY()+this->getHeight()/2-5);
	paintable->getDisplay()->println(this->text); 	
	
	
}

bool Checkbox::isChecked(){
	return checked;
}

void Checkbox::setChecked(bool checked){
	this->checked=checked;
	paintElement();
}

int Checkbox::handleTouch(int x,int y){

	
	if(x>=this->getX() && this->getX()+this->getWidth()<=x && y>=this->getY() && y<=this->getY()+this->getHeight()){
		setChecked(!isChecked());
		return 1;
	}else{
		return -1;
	}
}


Label::Label(String id,int x,int y,String text):ControlElement(id,x,y,30,30){
	this->text=text;
}

void Label::paintElement(){

	paintable->getDisplay()->setTextColor(TEXTCOLOR, BACKGROUND),
	paintable->getDisplay()->setTextSize(2);
	paintable->getDisplay()->setCursor(this->getX(),this->getY());
	paintable->getDisplay()->println(this->text); 	
	
	
}

int Label::handleTouch(int x,int y){
	if(this->x<=x && this->x+this->width>=x && y>=this->y && y<=this->y+this->height){
		return 1;
	}else{
		return -1;
	}
}

void Label::setText(String text){
	if(text!=this->text){
		int16_t tx;
		int16_t ty;
		uint16_t tw;
		uint16_t th;
		 
		String lText=this->getText()+"\n";
		int str_len = lText.length() + 1; 

		char char_array[str_len];
		paintable->getDisplay()->setTextSize(2);
		lText.toCharArray(char_array, str_len);
		
		paintable->getDisplay()->getTextBounds(char_array,0,0,&tx,&ty,&tw,&th);
		
		paintable->getDisplay()->fillRect(this->getX(),this->getY(),tw,th,BACKGROUND);
		this->text=text;
		this->paintElement();
		
	}
}

String Label::getText(){
	return this->text;
}

MessageBox::MessageBox(String caption,String text,int style,int buttonStyle):ControlElement("MessageBox",10,10,0,0){
	this->caption=caption;
	this->text=text;
	this->style=style;
	this->buttonStyle=buttonStyle;
	zIndex=1;
	switch(buttonStyle){
		case BTN_OK:
			buttonCount=1;
			break;
		case BTN_YESNO:
			buttonCount=2;
			break;
		case BTN_OKCANCEL:
			buttonCount=2;
			break;
		case BTN_YESNOCANCEL:
			buttonCount=3;
			break;
	}
}


MessageBox::MessageBox(String caption,String text):MessageBox(caption,text,MSGBOX_NONE,BTN_OK){
	
}
void MessageBox::paintElement(){
	if(this->visible){

		paintable->getDisplay()->fillRect(this->getX(),this->getY(),this->getWidth(),this->getHeight(),GRAY);
		paintable->getDisplay()->drawLine(this->getX(),this->getY(),this->getX()+this->getWidth(),this->getY(),WHITE);
		paintable->getDisplay()->drawLine(this->getX(),this->getY(),this->getX(),this->getY()+this->getHeight(),WHITE);
		paintable->getDisplay()->drawLine(this->getX(),this->getY()+this->getHeight(),this->getX()+this->getWidth(),this->getY()+this->getHeight(),DARKGRAY);
		paintable->getDisplay()->drawLine(this->getX()+this->getWidth(),this->getY(),this->getX()+this->getWidth(),this->getY()+this->getHeight(),DARKGRAY);
		paintable->getDisplay()->drawLine(this->getX(),this->getY()+20,this->getX()+this->getWidth(),this->getY()+20,WHITE);
		paintable->getDisplay()->drawLine(this->getX(),this->getY()+21,this->getX()+this->getWidth(),this->getY()+21,DARKGRAY);
		paintable->getDisplay()->setTextColor(TEXTCOLOR, GRAY),
		paintable->getDisplay()->setTextSize(1);
		paintable->getDisplay()->setCursor(this->getX()+5,this->getY()+5);
		paintable->getDisplay()->println(this->caption); 	
		
		if(this->style==MSGBOX_NONE){
			paintable->getDisplay()->setCursor(this->getX()+5,this->getY()+30);
		}else{
			paintable->getDisplay()->setCursor(this->getX()+5,this->getY()+30);
			paintable->getDisplay()->setTextSize(3);
			switch(this->style){
				case MSGBOX_WARNING:
				paintable->getDisplay()->setTextColor(RED, GRAY);
				paintable->getDisplay()->println("!");  
				break;
				case MSGBOX_INFORMATION:
				paintable->getDisplay()->setTextColor(BLUE, GRAY);
				paintable->getDisplay()->println("i");  
				break;
				case MSGBOX_QUESTION:
				paintable->getDisplay()->setTextColor(YELLOW, GRAY);
				paintable->getDisplay()->println("?");  
				break;
			}
			paintable->getDisplay()->setCursor(this->getX()+25,this->getY()+30);
			
		
		}
		paintable->getDisplay()->setTextSize(1);
		paintable->getDisplay()->setTextColor(TEXTCOLOR, GRAY);
		paintable->getDisplay()->println(this->text); 	
		
	
		for(int i=0;i<buttonCount;i++){
			buttons[i]->paintElement();
		}
	}
};
int MessageBox::handleTouch(int x,int y){
	for(int i=0;i<buttonCount;i++){
		if(buttons[i]->handleTouch(x,y)>-1){
			setVisible(false);

			return buttons[i]->getID().toInt();;
		}
	}

	return -1;
	
}

void MessageBox::setPaintable(Paintable *paintable){
	ControlElement::setPaintable(paintable);
	width=(int)paintable->getDisplay()->width()-20;
	height=(int)paintable->getDisplay()->height()-20;
	int buttonWidth=50;
	int buttonHeight=50;
	int buttonY=this->getY()+this->getHeight()-buttonHeight-10;
	if(buttonStyle==BTN_OK){
		int buttonX=this->getX()+this->getWidth()/2-buttonWidth/2;

		buttons[0]=new Button(String(RESULT_OK),buttonX,buttonY,buttonWidth,buttonHeight,"Ok");
		buttons[0]->setPaintable(paintable);
	}else
		if(buttonStyle==BTN_OKCANCEL){
			int buttonX=this->getX()+this->getWidth()/2-buttonWidth-10;
	
			buttons[0]=new Button(String(RESULT_OK),buttonX,buttonY,buttonWidth,buttonHeight,"Ok");
			buttons[0]->setPaintable(paintable);
			buttons[1]=new Button(String(RESULT_CANCEL),buttonX+buttonWidth+10,buttonY,buttonWidth,buttonHeight,"Cancel");
			buttons[1]->setPaintable(paintable);
		}else
		if(buttonStyle==BTN_YESNO){
			int buttonX=this->getX()+this->getWidth()/2-buttonWidth-10;
	
			buttons[0]=new Button(String(RESULT_YES),buttonX,buttonY,buttonWidth,buttonHeight,"Yes");
			buttons[0]->setPaintable(paintable);
			buttons[1]=new Button(String(RESULT_NO),buttonX+buttonWidth+10,buttonY,buttonWidth,buttonHeight,"No");
			buttons[1]->setPaintable(paintable);
		}else
		if(buttonStyle==BTN_YESNOCANCEL){
			int buttonX=this->getX()+this->getWidth()/2-buttonWidth-20-buttonWidth/2;
	
			buttons[0]=new Button(String(RESULT_YES),buttonX,buttonY,buttonWidth,buttonHeight,"Yes");
			buttons[0]->setPaintable(paintable);
			buttons[1]=new Button(String(RESULT_NO),buttonX+buttonWidth+10,buttonY,buttonWidth,buttonHeight,"No");
			buttons[1]->setPaintable(paintable);
			buttons[2]=new Button(String(RESULT_CANCEL),buttonX+2*(buttonWidth+10),buttonY,buttonWidth,buttonHeight,"Cancel");
			buttons[2]->setPaintable(paintable);
		}
	
	
}

void MessageBox::setVisible(bool visible){
	if(this->visible!=visible){
		this->visible=visible;
		if(visible){
			paintable->setZIndex(1);
			paintElement();
		}else{
			paintable->repaint();
			paintable->setZIndex(0);
		}
		
			
	}
}

Input::Input(String id,int x,int y,int width):ControlElement(id,x,y,width,30){

}

void Input::paintElement(){
 	paintable->getDisplay()->fillRect(this->getX(),this->getY(),this->getWidth(),this->getHeight(),LOWEREDSURFACE);
	paintable->getDisplay()->drawLine(this->getX(),this->getY(),this->getX()+this->getWidth(),this->getY(),SHADEDARK);
	paintable->getDisplay()->drawLine(this->getX(),this->getY(),this->getX(),this->getY()+this->getHeight(),SHADEDARK);
	paintable->getDisplay()->drawLine(this->getX(),this->getY()+this->getHeight(),this->getX()+this->getWidth(),this->getY()+this->getHeight(),SHADELIGHT);
	paintable->getDisplay()->drawLine(this->getX()+this->getWidth(),this->getY(),this->getX()+this->getWidth(),this->getY()+this->getHeight(),SHADELIGHT);
	paintable->getDisplay()->setTextColor(TEXTCOLOR, LOWEREDSURFACE),
	paintable->getDisplay()->setTextSize(2);
	paintable->getDisplay()->setCursor(this->getX()+2,this->getY()+6);
	
	String textToDraw=getText();
	int16_t tx;
	int16_t ty;
	uint16_t tw;
	uint16_t th;
	int str_len = textToDraw.length() + 1; 
	char char_array[str_len];
	textToDraw.toCharArray(char_array, str_len);
	paintable->getDisplay()->getTextBounds(char_array,0,0,&tx,&ty,&tw,&th);
		
	while(tw>getWidth()){
		textToDraw=textToDraw.substring(0,textToDraw.length()-1);
		int str_len = textToDraw.length() + 1; 
		char char_array[str_len];
		textToDraw.toCharArray(char_array, str_len);
		paintable->getDisplay()->getTextBounds(char_array,0,0,&tx,&ty,&tw,&th);
	
	}
	
	paintable->getDisplay()->println(textToDraw); 	
}

int Input::handleTouch(int x,int y){
	if(this->x<=x && this->x+this->width>=x && y>=this->y && y<=this->y+this->height){
		return 1;
	}else{
		return -1;
	}
}

bool Input::requestsInput(){
	return true;
}

String Input::getText(){
	return text;
}

void Input::setText(String text){
	this->text=text;
	paintElement();
}
