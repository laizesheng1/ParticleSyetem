#version 430 core 

in float color; 
out vec4 Out;

void main(){
	
	if (color < 0.1) {
		Out = mix(vec4(vec3(0.0),1.0), vec4(0.0,0.5,1.0,1.0), color*10.0);  
	} else if (color > 0.9) {
		Out = mix(vec4(0.6,0.05,0.0,1.0), vec4(vec3(0.0),1.0), (color-0.9)*10.0);  
	} else {
		Out = mix(vec4(0.0,0.5,1.0,1.0), vec4(0.6,0.05,0.0,1.0), color);
	}	
}