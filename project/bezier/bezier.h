#pragma once


#include <vector>


using namespace glm;

#define RESOLUTION 500

class Hull {
    public:
        // Construtor 
        Hull(vec3 p1 = vec3(0,0,0), vec3 p2 = vec3(0,0,0), vec3 p3 = vec3(0,0,0), vec3 p4 = vec3(0,0,0)){ 
        	_p1 = p1; 
        	_p2 = p2;
        	_p3 = p3; 
        	_p4 = p4;
        }
        // Getter 
        vec3& get_p1() { return _p1; }
        vec3& get_p2() { return _p2; }
        vec3& get_p3() { return _p3; }
       	vec3& get_p4() { return _p4; }
        
        vector<vec3> get_Hull() { return vector<vec3>{get_p1(), get_p2(), get_p3(), get_p4()}; }
            
    private:
        vec3 _p1;
        vec3 _p2;
        vec3 _p3;
        vec3 _p4;
};


class Bezier {
	public:

		void print_points(){
	    	for (unsigned int i = 0; i < _vertices.size(); i++){
	    		cout << " X : " << _vertices.at(i).x << " Y : " << _vertices.at(i).y << " Z : " << _vertices.at(i).z << endl; 
	    	}
	    }

	    void addHulls(vector<Hull> hulls) {
	    	// Add hull by hull
			clear(); 
			for (int i = 0; i < hulls.size(); ++i) {
				bezier(hulls[i]);
			}
		}

		vec3 get_currentPoint (int time) {
	        return _vertices.at(time); 
		}

		vector<vec3> getVertices() {
	    	return _vertices;
		}

		void clear() {
		    _vertices.clear();
		}


	private:  
	    vector<vec3> _vertices; 	//Point from the bezier curve

		void bezier(Hull& p){
			//add RESOLUTION+1 point to _vertices - each point are on the bezier curve 
	        for (float i = 0 ; i <= RESOLUTION ; ++i) {
	            float a = i/RESOLUTION;
	            _vertices.push_back(deCasteljau(p, a));
	        }

	        cout << "La taille de l'array est " << _vertices.size() << endl; 
	    }

	    vec3 deCasteljau(Hull &p, float a) {
	    	//compute the deCasteljau algorithm and return the point at time a 
	        vec3 p1 = linInterpolate(p.get_p1(), p.get_p2(), a);
	        vec3 p2 = linInterpolate(p.get_p2(), p.get_p3(), a);
	        vec3 p3 = linInterpolate(p.get_p3(), p.get_p4(), a);
	        p1 = linInterpolate(p1, p2, a);
	        p2 = linInterpolate(p2, p3, a);    
	        return linInterpolate(p1, p2, a);
	    }

	    vec3 linInterpolate(vec3 p1, vec3 p2, float a) {
	        return a * p2 + (1 - a) * p1;
	    }

		
};
