#include "gcode.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
//		Gcode Home Town
/////////////////////////////////////////////////////////////////////////////////////////////////

static int getCmd(char **data){
    char *ptr;
    int op;
    int opcode;
    ptr=*data;
    
    while(*ptr==' ') ptr++;
    
    if(*ptr=='\0'){
        *data=ptr;
        return GCODE_ERROR_NULL;
    }

    else if(*ptr=='\n'){
        *data=ptr;
        return GCODE_ERROR_ENDLINE;
    }
    
    else if(*ptr==';'){
        *data=ptr;
        return GCODE_ERROR_SEMICOLUMN;
    }

    else if(*ptr>='A' && *ptr<='Z'){
        op=(*ptr)<<24;
        ptr++;
        opcode=0;
        if(*ptr<'0' || *ptr>'9') return GCODE_ERROR_UNSUPPORTED_CMD;
        while(*ptr>='0' && *ptr<='9'){
            opcode=10*opcode+*ptr-'0';
            ptr++;
        }
        *data=ptr;
        if(*ptr!=' ' && *ptr!='\n' && *ptr!='\0' && *ptr!=';') return GCODE_ERROR_UNSUPPORTED_CMD;
        if((unsigned int)opcode&0xff000000)                    return GCODE_ERROR_UNSUPPORTED_CMD;
        return opcode|op;
    }
    return GCODE_ERROR_UNSUPPORTED_CMD;
}

static int getPar(char **data,float *parameter){
    int flg;
    float val;
    float dig=(float)0.1;
    char *ptr;
    int sign=1;
    int id;
    ptr=*data;
    *parameter=0;
    while(*ptr==' ') ptr++;   
    if(*ptr=='\0'){
        *data=ptr;
        return GCODE_ERROR_NULL;
    }
    else if(*ptr=='\n'){
        *data=ptr;
        return GCODE_ERROR_ENDLINE;
    }
    else if(*ptr==';'){
        *data=ptr;
        return GCODE_ERROR_SEMICOLUMN;
    }
    else if((*ptr>='A' && *ptr<='Z') || (*ptr>='a' && *ptr<='z')){
        id=*ptr;
        ptr++;
        if(*ptr==' ' || *ptr=='\n' || *ptr=='\0' || *ptr==';') return id;

        if(*ptr!='-' && *ptr!='.' && (*ptr<'0' || *ptr>'9')) return GCODE_ERROR_FORMAT;

        val=0;
        flg=0;
        sign=1;

        if(*ptr=='-') {sign=-1;ptr++;}
        
        while((*ptr>='0' && *ptr<='9' )|| *ptr=='.'){
            if(*ptr=='.'){
                if(flg==0) flg=1;
                else return GCODE_ERROR_FORMAT;
            }
            else if(flg==0) val=10*val+*ptr-'0';
            else{           val+=(*ptr-'0')*dig; dig/=10;}        
            ptr++;
        }    
        *data=ptr;
        *parameter=val*(float)sign;

        if(*ptr==' ' || *ptr=='\n' || *ptr=='\0' || *ptr==';') return id;
    }
    return GCODE_ERROR_FORMAT;
}


static int nextLine(char **data){
    char *ptr;
    ptr=*data;
    while(*ptr!='\n' && *ptr!='\0') ptr++;
    *data=ptr;
    if(*ptr=='\0') return GCODE_ERROR_NULL;
    if(*ptr=='\n')    ptr++; //going nextLine just go one next line no need to go through all empty lines
    *data=ptr;
    return GCODE_ERROR_ENDLINE;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////

int gCodeParse(char *inp,gCode *gout){
    int     cmd;
    int     par;
	 char **gcode;
    float   parf; 
	 gcode=&inp;
	 if(gout==0 || gcode==0 || *gcode==0) return GCODE_ERROR_NULL;
    gout->ccode=0;
	 gout->cword=0;
	 for(int i='A';i<='Z';i++) gout->key[gArg(i)]=0;
    cmd=getCmd(gcode);
    if      (cmd==GCODE_ERROR_NULL)              cmd=GCODE_ERROR_NULL;
    else if (cmd==GCODE_ERROR_SEMICOLUMN)        cmd=GCODE_ERROR_SEMICOLUMN;
    else if (cmd==GCODE_ERROR_ENDLINE)           cmd=GCODE_ERROR_ENDLINE;
    else if (cmd==GCODE_ERROR_UNSUPPORTED_CMD)   cmd=GCODE_ERROR_UNSUPPORTED_CMD;
    else if (cmd==GCODE_ERROR_FORMAT)            cmd=GCODE_ERROR_FORMAT;
    else{
		  gout->cword=(char)(cmd>>24);
		  gout->ccode=cmd&0xffffff;
		  cmd=GCODE_OK;
        while(1){
            par=getPar(gcode,&parf);
            if(par==GCODE_ERROR_SEMICOLUMN)      break;
            else if(par==GCODE_ERROR_ENDLINE)    break;
            else if(par==GCODE_ERROR_NULL)       break;
            else if(par==GCODE_ERROR_FORMAT)     {cmd=GCODE_ERROR_FORMAT;break;}
            else if(par>='A' && par<='Z')        {gout->key[gArg(par)]=1; gout->value[gArg(par)]=parf;}
            else                                 {cmd=GCODE_ERROR_FORMAT;break;}
            }
    }        
    nextLine(gcode);
    return cmd;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////