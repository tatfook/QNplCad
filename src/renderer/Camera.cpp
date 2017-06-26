 #include "Camera.h"
 Camera::Camera(enum CameraType camtype) :
 	type(camtype), projection(Camera::PERSPECTIVE), fov(22.5), viewall(false)
 {

 	// gimbal cam values
 	object_trans = QVector3D(0, 0, 0);
 	object_rot = QVector3D(35, 0, 25);
 	viewer_distance = 500;

 	// vector cam values
 	center = QVector3D(0, 0, 0);
	QVector3D cameradir(1, 1, -0.5);
 	eye = center - 500 * cameradir;

 	//pixel_width = RenderSettings::inst()->img_width;
 	//pixel_height = RenderSettings::inst()->img_height;
 	autocenter = false;
 }

 void Camera::setup(std::vector<double> params)
 {
 	if (params.size() == 7) {
 		type = Camera::GIMBAL;
 		object_trans = QVector3D(params[0], params[1], params[2]);
 		object_rot = QVector3D(params[3], params[4], params[5]);
 		viewer_distance = params[6];
 	}
 	else if (params.size() == 6) {
 		type = Camera::VECTOR;
 		eye = QVector3D(params[0], params[1], params[2]);
 		center = QVector3D(params[3], params[4], params[5]);
 	}
 	else {
		Q_ASSERT("Gimbal cam needs 7 numbers, Vector camera needs 6");
 	}
 }

 void Camera::gimbalDefaultTranslate()
 {	// match the GUI viewport numbers (historical reasons)
 	object_trans[0] *= -1;
 	object_trans[1] *= -1;
 	object_trans[2] *= -1;
 	object_rot[0] = fmodf(360 - object_rot.x() + 90, 360);
 	object_rot[1] = fmodf(360 - object_rot.y(), 360);
 	object_rot[2] = fmodf(360 - object_rot.z(), 360);
 }

 /*!
 Moves camera so that the given bbox is fully visible.
 */
 //void Camera::viewAll(const BoundingBox &bbox)
 //{
 //	if (this->type == Camera::NONE) {
 //		this->type = Camera::VECTOR;
 //		this->center = bbox.center();
 //		this->eye = this->center - Vector3d(1, 1, -0.5);
 //	}

 //	if (this->autocenter) {
 //		// autocenter = point camera at the center of the bounding box.
 //		if (this->type == Camera::GIMBAL) {
 //			this->object_trans = -bbox.center(); // for Gimbal cam
 //		}
 //		else if (this->type == Camera::VECTOR) {
 //			Vector3d dir = this->center - this->eye;
 //			this->center = bbox.center(); // for Vector cam
 //			this->eye = this->center - dir;
 //		}
 //	}

 //	double bboxRadius = bbox.diagonal().norm() / 2;
 //	double radius = (bbox.center() - this->center).norm() + bboxRadius;
 //	double distance = radius / sin(this->fov / 2 * M_PI / 180);
 //	switch (this->type) {
 //	case Camera::GIMBAL:
 //		this->viewer_distance = distance;
 //		break;
 //	case Camera::VECTOR: {
 //		Vector3d cameradir = (this->center - this->eye).normalized();
 //		this->eye = this->center - distance*cameradir;
 //		break;
 //	}
 //	default:
 //		assert(false && "Camera type not specified");
 //	}
 //}

 void Camera::zoom(int delta)
 {
 	this->viewer_distance *= pow(0.9, delta / 120.0);
 }

 void Camera::setProjection(ProjectionType type)
 {
 	this->projection = type;
 }

 void Camera::resetView()
 {
 	type = Camera::GIMBAL;
 	object_rot  = QVector3D(35, 0, -25);
 	object_trans = QVector3D(0, 0, 0);
 	viewer_distance = 140;
 }

 double Camera::zoomValue()
 {
 	return viewer_distance;
 }

 std::string Camera::statusText()
 {
 	/*boost::format fmt(_("Viewport: translate = [ %.2f %.2f %.2f ], rotate = [ %.2f %.2f %.2f ], distance = %.2f"));
 	fmt % object_trans.x() % object_trans.y() % object_trans.z()
 		% object_rot.x() % object_rot.y() % object_rot.z()
 		% viewer_distance;
 	return fmt.str();*/
 	return "";
 }
