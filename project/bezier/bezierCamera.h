#pragma once
#include <vector>
#include "bezier.h"

using namespace glm;


class BezierCamera {
	public:

	    BezierCamera(){};
	    
	    void BezCam(Camera &cam, int t){
		    vec3 eye = _eyeCurve.get_currentPoint(t);
		    vec3 viewDir = _viewDirCurve.get_currentPoint(t);
		    cam.Bezier(eye, viewDir); 		    
	    }

		void init(){
			bezier = false; 
			mode = 1; 
			index_min = 0; 
			index_max = 2 * (RESOLUTION) + 1; 
			
			vector<Hull> eyeHulls;
		    eyeHulls.clear();
		    vector<Hull> viewDirHulls;
		    viewDirHulls.clear();


		    //eyeHulls.push_back(Hull(vec3(-1, 0.2, -1.0),vec3(-0.7, 0.2, 0.7),vec3(0.0, 0.2, 0.4), vec3(0.6, 0.2, 0.2)));
		  	//viewDirHulls.push_back(Hull(vec3(0.5, 0.0, 0.5),vec3(0.5, 0.0, -0.5),vec3(-0.5, 0.0, 0.-0.3),vec3(-0.5, 0.0, 0.0)));
		    //eyeHulls.push_back(Hull(vec3(-1, 0.2, -1.0),vec3(-0.7, 0.2, 0.7),vec3(0.6, 0.2, 0.2), vec3(0.8, 0.2, -0.4)));
		  	//viewDirHulls.push_back(Hull(vec3(0.5, 0.0, 0.5),vec3(0.5, 0.0, -0.5),vec3(-0.5, 0.0, -0.5),vec3(-0.5, 0.0, 0.5)));
  
		    //eyeHulls.push_back(Hull(vec3(-1, 0.4, -1.0),vec3(1, 0.4, -1),vec3(1, 0.4, 1), vec3(-1, 0.4, 1)));
		    //viewDirHulls.push_back(Hull(vec3(0.3, -0.3, 0.7),vec3(-0.3, -0.2, 0.5),vec3(-0.1, 0.0, -0.6),vec3(0.4, -0.2, -0.7)));

		  	//Lente descente 
		  	//eyeHulls.push_back(Hull(vec3(-1, 1, -0.9),vec3(-0.5, 0.8, -0.9),vec3(0, 0.6, -0.9), vec3(0.5, 0.2, -0.9)));
		    //viewDirHulls.push_back(Hull(vec3(0.3, -0.3, 0.2),vec3(0.3, -0.5, 0.3),vec3(0.1, -0.5, 0.6),vec3(0.0, -0.2, 0.7)));

		    // bords eau
		    eyeHulls.push_back(Hull(vec3(-0.40, 0.05, -0.4),vec3(-0.2, 0.05, -0.35),vec3(-0.1, 0.05, -0.6), vec3(0.1, 0.05, -0.7)));
		    viewDirHulls.push_back(Hull(vec3(-0.3, 0.0, 0.4),vec3(0.4, 0.0, 0.7),vec3(0.7, 0.0, 0.4),vec3(0.9, 0.0, 0.0)));

		    
		    // Petit tour
		    eyeHulls.push_back(Hull(vec3(-0.6, 0.2, 0.5),vec3(-0.6, 0.2, 0.5),vec3(-0.6, 0.2, 0.5), vec3(-0.6, 0.2, 0.5)));
		    viewDirHulls.push_back(Hull(vec3(0.6, 0., 0.0),vec3(0.4, -0.1, -0.2),vec3(0.2, -0.1, -0.4),vec3(0.1, -0.1, -0.5)));
		    

		    // Vue 3
		    //eyeHulls.push_back(Hull(vec3(-1, 0.2, -1.0),vec3(-0.7, 0.8, 0.3),vec3(0.0, 0.4, 0.5), vec3(0.2, 0.2, 0.6)));
		    //viewDirHulls.push_back(Hull(vec3(0.3, 0.0, 1.0),vec3(1.0, -1.0, 0.7),vec3(0.9, -0.6, 0.4),vec3(0.7, -0.4, 0.0)));

		    // Vue 4 
		    //eyeHulls.push_back(Hull(vec3(1, 0.2, 1.0),vec3(0.7, 0.2, -0.3),vec3(-0.4, 0.2, -0.5), vec3(-0.6, 0.2, -0.6)));
		    //viewDirHulls.push_back(Hull(vec3(-0.3, 0.0, -1.0),vec3(-1.0, 0.0, -0.7),vec3(-1.0, 0.0, 0.0),vec3(0.3, 0.0, 1.0)));

		   
		    setHulls(eyeHulls, viewDirHulls);
		}

	    void clear(){
	    	_eyeCurve.clear();
   			_viewDirCurve.clear();
	    }

	    // Getter --------

	    bool get_bezier(){
	    	// Bezier is a bool indicate if we are in bezier mode 
	    	return bezier; 
	    }

	    void changeBezier(){
	    	if (bezier == true){
	    		bezier = false;
	    	} else {
	    		bezier = true; 
	    	}
	    }

	    void set_Mode(int i){
	    	// Mode is a int corresponding to the nb of the bezier path
	    	mode = i; 
	    }

	    void changeMode(int i){
	    	switch(i){
	    		case 1:
	    			index_min = 0; 
	    			index_max = RESOLUTION; 
	    			break; 
	    		case 2:
	    			index_min = RESOLUTION + 1; 
	    			index_max = 2*(RESOLUTION) + 1; 
	    			break; 

	    	}
	    }

	    int get_mode(){
	    	return mode; 
	    }

	    int get_indexMin(){
	    	return index_min; 
	    }

	    int get_indexMax(){
	    	return index_max; 
	    }
	   

	private:

		void setHulls(const vector<Hull> &eyeHulls, const vector<Hull> &viewDirHulls) {
	    	// From eye and viewDir hull set the bezier eyeCurve and viewDirCurve
		    clear();
		    _eyeHulls = eyeHulls;
		    _viewDirHulls = viewDirHulls;
		    _eyeCurve.addHulls(_eyeHulls);
		    _viewDirCurve.addHulls(_viewDirHulls);
		}
	   	
	   	int index_min; 
	   	int index_max; 
		int mode; 	   
	    bool bezier; 
	    Bezier _eyeCurve; 	// bezier for cam position
	    Bezier _viewDirCurve; 	// bezier for cam direction 
	    vector<Hull> _eyeHulls;		// All hull for cam position
	    vector<Hull> _viewDirHulls; 	// All hull for cam direction 
	    
};