#include <map>

#include <gmsh/Gmsh.h>
#include <gmsh/OpenFile.h>
#include <gmsh/GModel.h>
#include <gmsh/MElement.h>
#include <gmsh/VertexArray.h>
#include <gmsh/onelab.h>
#include <gmsh/onelabUtils.h>
#include <gmsh/PView.h>
#include <gmsh/PViewOptions.h>
#include <gmsh/PViewData.h>
#include <gmsh/Context.h>
#include <gmsh/StringUtils.h>
#include <getdp/GetDP.h>

#if defined(BUILD_ANDROID)
#include <GLES/gl.h>
#include <GLES/glext.h>
#include "androidGModel.h"
#else // iOS
#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#endif

#include "drawContext.h"
#include "drawString.h"
#include "Trackball.h"

static bool locked = false;
static bool onelabStop = false;

drawContext::drawContext(float fontFactor, bool retina)
{
  GmshInitialize();
  GmshSetOption("General", "Terminal", 1.0);
  onelabUtils::setFirstComputationFlag(false);
  for(int i = 0; i < 3; i++){
    _translate[i] = 0.;
    _scale[i] = 1.;
  }
  setQuaternion(0., 0., 0., 1.);
  _fontFactor = fontFactor;
  _retina = retina;
}

static void checkGlError(const char* op)
{
  //for (GLint error = glGetError(); error; error = glGetError())
  //  Msg::Error("%s: glError (0x%x)",op,error);
}

void drawContext::load(std::string filename)
{
  if(locked) return;

  // delete all models and post-processing views
  GmshClearProject();

  // reset onelab database
  onelab::server::instance()->clear();

  // restore default options
  GmshRestoreDefaultOptions();

  // output messages on console
  GmshSetOption("General", "Terminal", 1.0);

  // open the file with Gmsh
  GmshOpenProject(filename);

  // reset openGL view
  eventHandler(10);

  // run onelab clients to populate the database
  onelab_cb("check");

  // mark all parameters as changed to force complete first run
  onelab::server::instance()->setChanged(true);
}

void drawContext::eventHandler(int event, float x, float y)
{
  int width = _width, height = _height;
  if(_retina){ // x,y for retina are still the same as for non-retina
    width /= 2;
    height /= 2;
  }

  _current.set(_scale, _translate, _right, _left,
               _bottom, _top, width, height, x, y);
  double xx[3] = {1.,0.,0.};
  double yy[3] = {0.,1.,0.};
  double q[4];
  switch(event){
  case 0: // finger(s) press the screen
    // in this case x and y represent the start point
    _start.set(_scale, _translate, _right, _left,
               _bottom, _top, width, height, x, y);
    _previous.set(_scale, _translate, _right, _left,
                  _bottom, _top, width, height, x, y);
    break;
  case 1: // finger move (translate)
    // in this case x and y represent the current point
    _translate[0] += (_current.wnr[0] - _previous.wnr[0]);
    _translate[1] += (_current.wnr[1] - _previous.wnr[1]);
    _translate[2] = 0.;
    break;
  case 2: // fingers move (scale)
    // in this case we don't care about previous and current position, x
    // represent the scale
    _scale[0] = _scale[1] = _scale[2] = x;
    _start.recenter(_scale, _translate);
    break;
  case 3: // fingers move (rotate)
    addQuaternion((2. * _previous.win[0] - width) / width,
                  (height - 2. * _previous.win[1]) / height,
                  (2. * _current.win[0] - width) / width,
                  (height - 2. * _current.win[1]) / height);
    break;
  case 4: // release the finger(s)
    // Do nothing ?
    break;
  case 5: // X view
    axis_to_quat(xx, M_PI/2, q);
    setQuaternion(q[0], q[1], q[2], q[3]);
    break;
  case 6: // Y view
    axis_to_quat(yy, M_PI/2, q);
    setQuaternion(q[0], q[1], q[2], q[3]);
    break;
  case 7: // Z view
    setQuaternion(0., 0., 0., 1.);
    break;
  default: // all other reset the position
    setQuaternion(0., 0., 0., 1.);
    for(int i = 0; i < 3; i++){
      _translate[i] = 0.;
      _scale[i] = 1.;
    }
    break;
  }
  _previous.set(_scale, _translate, _right, _left,
                _bottom, _top, width, height, x, y);
}

void drawContext::setQuaternion(double q0, double q1, double q2, double q3)
{
  _quaternion[0] = q0;
  _quaternion[1] = q1;
  _quaternion[2] = q2;
  _quaternion[3] = q3;
}

void drawContext::addQuaternion(double p1x, double p1y, double p2x, double p2y)
{
  double quat[4];
  trackball(quat, p1x, p1y, p2x, p2y);
  add_quats(quat, _quaternion, _quaternion);
}

void drawContext::buildRotationMatrix()
{
  build_rotmatrix(_rotate, _quaternion);
  for(int i = 0; i < 16; i++)
    _rotatef[i] = (float)_rotate[i];
}

void drawContext::OrthofFromGModel()
{
  double Va = (double)_height / (double)_width;
  double Wa = (CTX::instance()->max[1] - CTX::instance()->min[1]) /
    (CTX::instance()->max[0] - CTX::instance()->min[0]);
  double vxmin, vxmax, vymin, vymax;
  if(Va > Wa) {
    vxmin = CTX::instance()->min[0];
    vxmax = CTX::instance()->max[0];
    vymin = 0.5 * (CTX::instance()->min[1] + CTX::instance()->max[1] -
                   Va * (CTX::instance()->max[0] - CTX::instance()->min[0]));
    vymax = 0.5 * (CTX::instance()->min[1] + CTX::instance()->max[1] +
                   Va * (CTX::instance()->max[0] - CTX::instance()->min[0]));
  }
  else {
    vxmin = 0.5 * (CTX::instance()->min[0] + CTX::instance()->max[0] -
                   (CTX::instance()->max[1] - CTX::instance()->min[1]) / Va);
    vxmax = 0.5 * (CTX::instance()->min[0] + CTX::instance()->max[0] +
                   (CTX::instance()->max[1] - CTX::instance()->min[1]) / Va);
    vymin = CTX::instance()->min[1];
    vymax = CTX::instance()->max[1];
  }
  double fact = CTX::instance()->displayBorderFactor;
  double xborder = fact * (vxmax - vxmin), yborder = fact * (vymax - vymin);
  vxmin -= xborder;
  vxmax += xborder;
  vymin -= yborder;
  vymax += yborder;

  // set up the near and far clipping planes so that the box is large enough to
  // manipulate the model and zoom, but not too big (otherwise the z-buffer
  // resolution e.g. with Mesa can become insufficient)
  double zmax = std::max(fabs(CTX::instance()->min[2]),
                         fabs(CTX::instance()->max[2]));
  if(zmax < CTX::instance()->lc) zmax = CTX::instance()->lc;
  double clip_near = -zmax * _scale[2] * CTX::instance()->clipFactor;
  double clip_far = -clip_near;

  GLint matrixMode;
  glGetIntegerv(GL_MATRIX_MODE, &matrixMode);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrthof(vxmin, vxmax, vymin, vymax, clip_near, clip_far);
  glMatrixMode(matrixMode);

  _left = vxmin;
  _right = vxmax;
  _top = vymax;
  _bottom = vymin;
  _far = clip_far;
}

void drawContext::initView(int w, int h)
{
  _height = h;
  _width = w;
  glViewport(0, 0, w, h);

  OrthofFromGModel();

  glClearColor(.83, .85, .98, 1.);
  glDepthMask(GL_TRUE);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glDepthFunc(GL_LESS);
}

void drawArray(VertexArray *va, GLint type, bool useColorArray, bool useNormalArray)
{
  if(!va) return;
  glEnable(GL_BLEND);
  glEnable(GL_RESCALE_NORMAL);
  glShadeModel(GL_SMOOTH);
  glVertexPointer(3, GL_FLOAT, 0, va->getVertexArray());
  glEnableClientState(GL_VERTEX_ARRAY);
  if(useNormalArray){
    glNormalPointer(GL_BYTE, 0, va->getNormalArray());
    glEnableClientState(GL_NORMAL_ARRAY);
  }
  if(useColorArray){
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, va->getColorArray());
    glEnableClientState(GL_COLOR_ARRAY);
  }
  glDrawArrays(type, 0, va->getNumVertices());
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
  glDisable(GL_RESCALE_NORMAL);
  glDisable(GL_BLEND);
}

void drawVector(double x, double y, double z, double dx, double dy, double dz)
{
  double l = sqrt(dx * dx + dy * dy + dz * dz), lt;
  double n[3], t[3], u[3];

  if(l == 0.0) return;

  GLfloat line[] = {
    (GLfloat)x, (GLfloat)y, (GLfloat)z,
    (GLfloat)(x+dx), (GLfloat)(y+dy), (GLfloat)(z+dz),
  };
  glVertexPointer(3, GL_FLOAT, 0, line);
  glEnableClientState(GL_VERTEX_ARRAY);
  glDrawArrays(GL_LINES, 0, 2);
  glDisableClientState(GL_VERTEX_ARRAY);

  n[0] = dx / l;
  n[1] = dy / l;
  n[2] = dz / l;

  if((fabs(n[0]) >= fabs(n[1]) && fabs(n[0]) >= fabs(n[2])) ||
     (fabs(n[1]) >= fabs(n[0]) && fabs(n[1]) >= fabs(n[2]))) {
    t[0] = n[1];
    t[1] = -n[0];
    t[2] = 0.;
  }
  else {
    t[0] = 0.;
    t[1] = n[2];
    t[2] = -n[1];
  }

  lt = sqrt(t[0] * t[0] + t[1] * t[1] + t[2] * t[2]);
  t[0] /= lt;
  t[1] /= lt;
  t[2] /= lt;

  u[0] = n[1] * t[2] - n[2] * t[1];
  u[1] = n[2] * t[0] - n[0] * t[2];
  u[2] = n[0] * t[1] - n[1] * t[0];

  lt = sqrt(u[0] * u[0] + u[1] * u[1] + u[2] * u[2]);
  u[0] /= lt;
  u[1] /= lt;
  u[2] /= lt;

  double f1 = 0.75; // Stem lenght
  double b = 0.1 * l;

  GLfloat arrow[] = {
    (GLfloat)(x + dx), (GLfloat)(y + dy), (GLfloat)(z + dz),
    (GLfloat)(x + f1 * dx + b * (t[0])), (GLfloat)(y + f1 * dy + b * (t[1])),
    (GLfloat)(z + f1 * dz + b * (t[2])),
    (GLfloat)(x + f1 * dx + b * (-t[0])), (GLfloat)(y + f1 * dy + b * (-t[1])),
    (GLfloat)(z + f1 * dz + b * (-t[2])),

    (GLfloat)(x + dx), (GLfloat)(y + dy), (GLfloat)(z + dz),
    (GLfloat)(x + f1 * dx + b * (-u[0])), (GLfloat)(y + f1 * dy + b * (-u[1])),
    (GLfloat)(z + f1 * dz + b * (-u[2])),
    (GLfloat)(x + f1 * dx + b * (u[0])), (GLfloat)(y + f1 * dy + b * (u[1])),
    (GLfloat)(z + f1 * dz + b * (u[2])),
  };
  glVertexPointer(3, GL_FLOAT, 0, arrow);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnable(GL_LINE_SMOOTH);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisable(GL_LINE_SMOOTH);
}

void drawContext::drawVectorArray(PViewOptions *opt, VertexArray *va)
{
  if(!va || va->getNumVerticesPerElement() != 2) return;

  for(int i = 0; i < va->getNumVertices(); i += 2){
    float *s = va->getVertexArray(3 * i);
    float *v = va->getVertexArray(3 * (i + 1));
    double l = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    double lmax = opt->tmpMax;
    if((l && opt->vectorType) && lmax){
      double scale = (opt->arrowSizeMax - opt->arrowSizeMin) / lmax;
      if(opt->arrowSizeMin && l) scale += opt->arrowSizeMin / l;
      double dx = scale * v[0];
      double dy = scale * v[1];
      double dz = scale * v[2];
      GLubyte *color = (GLubyte *)va->getColorArray(4 * i);
      glColor4ub(*(color), *(color+1), *(color+2), *(color+3));
      if(fabs(dx) > 1. || fabs(dy) > 1. || fabs(dz) > 1.){
        double d = (_right - _left) / _width / _scale[0];
        dx *= d; dy *= d; dz *= d;
        double x = s[0], y = s[1], z = s[2];
        drawVector(x,y,z,dx,dy,dz);
      }
    }
  }
}

void drawContext::drawPView(PView *p)
{
  PViewOptions *opt = p->getOptions();
  if(!opt->visible) return;

  glPointSize((GLfloat)opt->pointSize);
  glLineWidth((GLfloat)opt->lineWidth);

  drawArray(p->va_points, GL_POINTS, true);

  drawArray(p->va_lines, GL_LINES, true);

  glEnable(GL_LIGHTING);
  drawArray(p->va_triangles, GL_TRIANGLES, true, true);
  glDisable(GL_LIGHTING);

  glLineWidth(1);
  glPointSize(1);

  drawVectorArray(p->getOptions(), p->va_vectors);
}

void drawContext::drawScale()
{
  glPushMatrix();
  glLoadIdentity();

  double size = std::max(_right -_left, _top - _bottom);
  double width = size / 3.5;
  double height = size / 10.;
  double dh = height / 5;

  // Draw the scale bar
  int nPview = 0;
  for(int i=0; i<PView::list.size();i++){
    PView *p = PView::list[i];
    PViewOptions *opt = p->getOptions();
    if(!opt->visible) continue;
    PViewData *data = p->getData();

    double box = width / (opt->nbIso ? opt->nbIso : 1);
    double xmin = _left + (_right - _left - width)/2.;
    double ymin = _bottom + 0.7 * height + height * nPview;

    std::vector<GLfloat> vertex(opt->nbIso*3*4);
    std::vector<GLubyte> color(opt->nbIso*4*4);
    for(int i = 0; i < opt->nbIso; i++){
      if(opt->intervalsType == PViewOptions::Discrete ||
         opt->intervalsType == PViewOptions::Numeric){
        unsigned int col = opt->getColor(i, opt->nbIso);
        color[i*4*4+0] = color[i*4*4+4] = color[i*4*4+8] = color[i*4*4+12] =
          (GLubyte)CTX::instance()->unpackRed(col);
        color[i*4*4+1] = color[i*4*4+5] = color[i*4*4+9] = color[i*4*4+13] =
          (GLubyte)CTX::instance()->unpackGreen(col);
        color[i*4*4+2] = color[i*4*4+6] = color[i*4*4+10] = color[i*4*4+14] =
          (GLubyte)CTX::instance()->unpackBlue(col);
        color[i*4*4+3] = color[i*4*4+7] = color[i*4*4+11] = color[i*4*4+15] =
          (GLubyte)CTX::instance()->unpackAlpha(col);
        vertex[i*3*4+0] = xmin + i * box;
        vertex[i*3*4+1] = ymin;
        vertex[i*3*4+2] = 0.;
        vertex[i*3*4+3] = xmin + i * box;
        vertex[i*3*4+4] = ymin + dh;
        vertex[i*3*4+5] = 0.;
        vertex[i*3*4+6] = xmin + (i + 1) * box;
        vertex[i*3*4+7] = ymin;
        vertex[i*3*4+8] = 0.;
        vertex[i*3*4+9] = xmin + (i + 1) * box;
        vertex[i*3*4+10] = ymin + dh;
        vertex[i*3*4+11] = 0.;
      }
      else if(opt->intervalsType == PViewOptions::Continuous){
        double dv = (opt->tmpMax - opt->tmpMin) / (opt->nbIso ? opt->nbIso : 1);
        double v1 = opt->tmpMin + i * dv;
        unsigned int col1 = opt->getColor(v1, opt->tmpMin, opt->tmpMax, true);
        color[i*4*4+0] = color[i*4*4+4] = (GLubyte)CTX::instance()->unpackRed(col1);
        color[i*4*4+1] = color[i*4*4+5] = (GLubyte)CTX::instance()->unpackGreen(col1);
        color[i*4*4+2] = color[i*4*4+6] = (GLubyte)CTX::instance()->unpackBlue(col1);
        color[i*4*4+3] = color[i*4*4+7] = (GLubyte)CTX::instance()->unpackAlpha(col1);
        vertex[i*3*4+0] = xmin + i * box;
        vertex[i*3*4+1] = ymin;
        vertex[i*3*4+2] = 0.;
        vertex[i*3*4+3] = xmin + i * box;
        vertex[i*3*4+4] = ymin + dh;
        vertex[i*3*4+5] = 0.;
        double v2 = opt->tmpMin + (i + 1) * dv;
        unsigned int col2 = opt->getColor(v2, opt->tmpMin, opt->tmpMax, true);
        color[i*4*4+8] = color[i*4*4+12] = (GLubyte)CTX::instance()->unpackRed(col2);
        color[i*4*4+9] = color[i*4*4+13] = (GLubyte)CTX::instance()->unpackGreen(col2);
        color[i*4*4+10] = color[i*4*4+14] = (GLubyte)CTX::instance()->unpackBlue(col2);
        color[i*4*4+11] = color[i*4*4+15] = (GLubyte)CTX::instance()->unpackAlpha(col2);
        vertex[i*3*4+6] = xmin + (i + 1) * box;
        vertex[i*3*4+7] = ymin;
        vertex[i*3*4+8] = 0.;
        vertex[i*3*4+9] = xmin + (i + 1) * box;
        vertex[i*3*4+10] = ymin + dh;
        vertex[i*3*4+11] = 0.;
      }
      else{
        unsigned int col = opt->getColor(i, opt->nbIso);
        color[i*4*4+0] = color[i*4*4+4] = color[i*4*4+8] = color[i*4*4+12] =
          (GLubyte)CTX::instance()->unpackRed(col);
        color[i*4*4+1] = color[i*4*4+5] = color[i*4*4+9] = color[i*4*4+13] =
          (GLubyte)CTX::instance()->unpackGreen(col);
        color[i*4*4+2] = color[i*4*4+6] = color[i*4*4+10] = color[i*4*4+14] =
          (GLubyte)CTX::instance()->unpackBlue(col);
        color[i*4*4+3] = color[i*4*4+7] = color[i*4*4+11] = color[i*4*4+15] =
          (GLubyte)CTX::instance()->unpackAlpha(col);
        vertex[i*3*4+0] = xmin + i * box;
        vertex[i*3*4+1] = ymin;
        vertex[i*3*4+2] = 0.;
        vertex[i*3*4+3] = xmin + i * box;
        vertex[i*3*4+4] = ymin + dh;
        vertex[i*3*4+5] = 0.;
        vertex[i*3*4+6] = xmin + (i + 1) * box;
        vertex[i*3*4+7] = ymin;
        vertex[i*3*4+8] = 0.;
        vertex[i*3*4+9] = xmin + (i + 1) * box;
        vertex[i*3*4+10] = ymin + dh;
        vertex[i*3*4+11] = 0.;
      }
    }

    glVertexPointer(3, GL_FLOAT, 0, &vertex[0]);
    glEnableClientState(GL_VERTEX_ARRAY);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, &color[0]);
    glEnableClientState(GL_COLOR_ARRAY);
    if(opt->intervalsType == PViewOptions::Discrete ||
       opt->intervalsType == PViewOptions::Numeric ||
       opt->intervalsType == PViewOptions::Continuous)
      glDrawArrays(GL_TRIANGLE_STRIP, 0, opt->nbIso*4);
    else
      glDrawArrays(GL_LINES, 0, opt->nbIso*4);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

    char label[1024];
    int nt = data->getNumTimeSteps();
    if((opt->showTime == 1 && nt > 1) || opt->showTime == 2){
      char tmp[256];
      sprintf(tmp, opt->format.c_str(), data->getTime(opt->timeStep));
      sprintf(label, "%s (%s)", data->getName().c_str(), tmp);
    }
    else if((opt->showTime == 3 && nt > 1) || opt->showTime == 4){
      sprintf(label, "%s (%d/%d)", data->getName().c_str(), opt->timeStep,
              data->getNumTimeSteps() - 1);
    }
    else{
      sprintf(label, "%s", data->getName().c_str());
    }
    drawString lbl(label, 20 * _fontFactor);
    lbl.draw(xmin + width / 2, ymin + 2.8 * dh, 0.,
             _width/(_right-_left), _height/(_top-_bottom));

    drawString val(data->getName().c_str(), 15 * _fontFactor);
    for(int i = 0; i < 3; i++) {
      double v = opt->getScaleValue(i, 3, opt->tmpMin, opt->tmpMax);
      sprintf(label, opt->format.c_str(), v);
      val.setText(label);
      val.draw(xmin + i * width/ 2, ymin + 1.5 * dh, 0.,
               _width/(_right-_left), _height/(_top-_bottom));
    }
    nPview++;
  }
  glPopMatrix();
}

void drawContext::drawPost()
{
  if(PView::list.empty()) return ;

  for(unsigned int i = 0; i < PView::list.size(); i++){
    PView::list[i]->fillVertexArrays();
    drawPView(PView::list[i]);
  }
}

void drawContext::drawAxes()
{
  glLineWidth(1.);
  glPushMatrix();
  glLoadIdentity();

  GLfloat h = std::max(_top - _bottom, _right - _left) / 25. ;
  GLfloat x0 = _right - 1.8 * h;
  GLfloat y0 = _bottom + 1.5 * h;

  GLfloat xx = h * _rotatef[0];
  GLfloat xy = h * _rotatef[1];
  GLfloat yx = h * _rotatef[4];
  GLfloat yy = h * _rotatef[5];
  GLfloat zx = h * _rotatef[8];
  GLfloat zy = h * _rotatef[9];
  GLfloat o = h / 10;

  const GLfloat axes[] = {
    x0, y0,
    x0 + xx, y0 + xy,
    x0, y0,
    x0 + yx, y0 + yy,
    x0, y0,
    x0 + zx, y0 + zy
  };
  GLfloat colors[] = {
    0., 0, 0, 1.,
    0., 0, 0, 1.,
    0, 0, 0., 1.,
    0, 0, 0., 1.,
    0, 0., 0, 1.,
    0, 0., 0, 1.,
  };
  glVertexPointer(2, GL_FLOAT, 0, axes);
  glEnableClientState(GL_VERTEX_ARRAY);
  glColorPointer(4, GL_FLOAT, 0, colors);
  glEnableClientState(GL_COLOR_ARRAY);
  glDrawArrays(GL_LINES, 0, 6);
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);

  drawString x("X", 15 * _fontFactor, colors);
  x.draw(x0 + xx + o, y0 + xy + o, 0, _width/(_right-_left), _height/(_top-_bottom), false);
  drawString y("Y", 15 * _fontFactor, colors+8);
  y.draw(x0 + yx + o, y0 + yy + o, 0, _width/(_right-_left), _height/(_top-_bottom), false);
  drawString z("Z", 15 * _fontFactor, colors+16);
  z.draw(x0 + zx + o, y0 + zy + o, 0, _width/(_right-_left), _height/(_top-_bottom), false);
  glPopMatrix();
}

int drawContext::fix2dCoordinates(double *x, double *y)
{
  int ret = (*x > 99999 && *y > 99999) ? 3 : (*y > 99999) ? 2 : (*x > 99999) ? 1 : 0;

  if(*x < 0) // measure from right border
    *x = _right + *x;
  else if(*x > 99999) // by convention, x-centered
    *x = _right / 2;

  if(*y < 0) // measure from bottom border
    *y = -(*y);
  else if(*y > 99999) // by convention, y-centered
    *y = _top / 2.;
  else
    *y = _top - *y;
  return ret;
}

void drawContext::drawText2d()
{
  glPushMatrix();
  glLoadIdentity();

  for(unsigned int i = 0; i < PView::list.size(); i++){
    PViewData *data = PView::list[i]->getData();
    PViewOptions *opt = PView::list[i]->getOptions();
    if(opt->visible && opt->drawStrings){
      for(int j = 0; j < data->getNumStrings2D(); j++){
        double x, y, style;
        std::string str;
        data->getString2D(j, opt->timeStep, str, x, y, style);
        //fix2dCoordinates(&x, &y);
        GLfloat colors[] = {0., 0, 0, 1.};
        drawString s(str.c_str(), 20 * _fontFactor, colors);
        // FIXME:
        s.draw(_left + (_right - _left) / 2.,
               _bottom + 0.8 * (_top - _bottom), 0,
               _width/(_right-_left), _height/(_top-_bottom), true);
      }
    }
  }

  glPopMatrix();
}

void drawGraph2d()
{
  glPushMatrix();
  glLoadIdentity();

  std::vector<PView*> graphs;
  for(unsigned int i = 0; i < PView::list.size(); i++){
    PViewData *data = PView::list[i]->getData();
    PViewOptions *opt = PView::list[i]->getOptions();
    if(!data->getDirty() && opt->visible && opt->type != PViewOptions::Plot3D)
      graphs.push_back(PView::list[i]);
  }
  if(graphs.empty()) return;

  // FIXME: draw 2d graph(s)

  glPopMatrix();
}

void drawContext::drawView()
{
  OrthofFromGModel();

  glMatrixMode(GL_MODELVIEW);

  // draw the background
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  if(CTX::instance()->bgGradient){
    glPushMatrix();
    glLoadIdentity();
    const GLfloat squareVertices[] = {
      (GLfloat)_top,	(GLfloat)_left, 2*_far,
      (GLfloat)_top,	(GLfloat)_right, 2*_far,
      (GLfloat)_bottom,	(GLfloat)_left, 2*_far,
      (GLfloat)_bottom,	(GLfloat)_right, 2*_far,
    };
    const GLubyte squareColors[] = {
      255, 255, 255, 255,
      255, 255, 255, 255,
      190, 200, 255, 255,
      190, 200, 255, 255,
    };
    glVertexPointer(3, GL_FLOAT, 0, squareVertices);
    glEnableClientState(GL_VERTEX_ARRAY);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, squareColors);
    glEnableClientState(GL_COLOR_ARRAY);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glPopMatrix();
  }
  checkGlError("Draw background");

  // init lights
  glPushMatrix();
  glLoadIdentity();
  glScalef(_scale[0], _scale[1], _scale[2]);
  glTranslatef(_translate[0], _translate[1], _translate[2]);
  for(int i = 0; i < 6; i++) {
    if(CTX::instance()->light[i]) {
      GLfloat position[4] = {
        (GLfloat)CTX::instance()->lightPosition[i][0],
        (GLfloat)CTX::instance()->lightPosition[i][1],
        (GLfloat)CTX::instance()->lightPosition[i][2],
        (GLfloat)CTX::instance()->lightPosition[i][3]
      };
      glLightfv((GLenum)(GL_LIGHT0 + i), GL_POSITION, position);
      GLfloat r = (GLfloat)(CTX::instance()->unpackRed
                            (CTX::instance()->color.ambientLight[i]) / 255.);
      GLfloat g = (GLfloat)(CTX::instance()->unpackGreen
                            (CTX::instance()->color.ambientLight[i]) / 255.);
      GLfloat b = (GLfloat)(CTX::instance()->unpackBlue
                            (CTX::instance()->color.ambientLight[i]) / 255.);
      GLfloat ambient[4] = {r, g, b, 1.0F};
      glLightfv((GLenum)(GL_LIGHT0 + i), GL_AMBIENT, ambient);
      r = (GLfloat)(CTX::instance()->unpackRed
                    (CTX::instance()->color.diffuseLight[i]) / 255.);
      g = (GLfloat)(CTX::instance()->unpackGreen
                    (CTX::instance()->color.diffuseLight[i]) / 255.);
      b = (GLfloat)(CTX::instance()->unpackBlue
                    (CTX::instance()->color.diffuseLight[i]) / 255.);
      GLfloat diffuse[4] = {r, g, b, 1.0F};
      glLightfv((GLenum)(GL_LIGHT0 + i), GL_DIFFUSE, diffuse);
      r = (GLfloat)(CTX::instance()->unpackRed
                    (CTX::instance()->color.specularLight[i]) / 255.);
      g = (GLfloat)(CTX::instance()->unpackGreen
                    (CTX::instance()->color.specularLight[i]) / 255.);
      b = (GLfloat)(CTX::instance()->unpackBlue
                    (CTX::instance()->color.specularLight[i]) / 255.);
      GLfloat specular[4] = {r, g, b, 1.0F};
      glLightfv((GLenum)(GL_LIGHT0 + i), GL_SPECULAR, specular);
      glEnable((GLenum)(GL_LIGHT0 + i));
    }
    else{
      glDisable((GLenum)(GL_LIGHT0 + i));
    }
  }
  glPopMatrix();
  // ambient and diffuse material colors track glColor automatically
  //glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);
  // "white"-only specular material reflection color
  GLfloat spec[4] = {(GLfloat)CTX::instance()->shine,
                     (GLfloat)CTX::instance()->shine,
                     (GLfloat)CTX::instance()->shine, 1.0F};
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
  // specular exponent in [0,128] (larger means more "focused"
  // reflection)
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS,
              (GLfloat)CTX::instance()->shineExponent);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_RESCALE_NORMAL);
  glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 1.);
  glDisable(GL_LIGHTING);
  checkGlError("Initialize lights");

  // init position
  glLoadIdentity();
  glScalef(_scale[0], _scale[1], _scale[2]);
  glTranslatef(_translate[0], _translate[1], _translate[2]);
  if(CTX::instance()->rotationCenterCg)
    glTranslatef(CTX::instance()->cg[0],
                 CTX::instance()->cg[1],
                 CTX::instance()->cg[2]);
  else
    glTranslatef(CTX::instance()->rotationCenter[0],
                 CTX::instance()->rotationCenter[1],
                 CTX::instance()->rotationCenter[2]);
  buildRotationMatrix();
  glMultMatrixf(_rotatef);
  if(CTX::instance()->rotationCenterCg)
    glTranslatef(-CTX::instance()->cg[0],
                 -CTX::instance()->cg[1],
                 -CTX::instance()->cg[2]);
  else
    glTranslatef(-CTX::instance()->rotationCenter[0],
                 -CTX::instance()->rotationCenter[1],
                 -CTX::instance()->rotationCenter[2]);
  checkGlError("Initialize position");

  // draw everything
  glEnable(GL_DEPTH_TEST);
  drawMesh(); checkGlError("Draw mesh");
  drawGeom(); checkGlError("Draw geometry");
  drawPost(); checkGlError("Draw post-pro");
  glDisable(GL_DEPTH_TEST);
  drawScale(); checkGlError("Draw scales");
  drawAxes(); checkGlError("Draw axes");
  drawText2d(); checkGlError("Draw text2d");
  drawGraph2d(); checkGlError("Draw graph2d");
}

std::vector<std::string> commandToVector(const std::string cmd)
{
  std::vector<std::string> ret;
  int pos = 0, last = 0;
  while((pos = cmd.find("-", last+1)) != std::string::npos){
    ret.push_back(cmd.substr(last,pos-1-last));
    last = pos;
  }
  ret.push_back(cmd.substr(last));
  return ret;
}

int onelab_cb(std::string action)
{
  if(action == "stop"){
    onelab::string o("GetDP/Action", "stop");
    o.setVisible(false);
    o.setNeverChanged(true);
    onelab::server::instance()->set(o);
    onelabStop = true;
    return 0;
  }

  if(locked) return -1;
  locked = true;

  if(action == "reset"){
    onelab::server::instance()->clear();
    onelabUtils::runGmshClient(action, true);
    action = "check";
  }

  Msg::ResetErrorCounter();

  if(action == "compute"){
    onelabUtils::initializeLoop("1");
    onelabUtils::initializeLoop("2");
    onelabUtils::initializeLoop("3");
  }

  do{
    // run Gmsh (only if necessary)
    onelabUtils::runGmshClient(action, true);

    // run GetDP (always -- to not confuse the user)
    onelab::string o("GetDP/Action", action);
    o.setVisible(false);
    o.setNeverChanged(true);
    onelab::server::instance()->set(o);

    std::vector<std::string> args;
    args.push_back("getdp");
    std::vector<onelab::string> ps;
    onelab::server::instance()->get(ps, "GetDP/1ModelName");
    if(ps.empty()){
      std::vector<std::string> split = SplitFileName(GModel::current()->getFileName());
      std::string name(split[0] + split[1]);
      onelab::string o("GetDP/1ModelName", name, "Model name");
      o.setKind("file");
      onelab::server::instance()->set(o);
      ps.push_back(o);
    }
    args.push_back(ps[0].getValue());
    if(action == "check")
      onelab::server::instance()->get(ps, "GetDP/9CheckCommand");
    else if(action == "compute")
      onelab::server::instance()->get(ps, "GetDP/9ComputeCommand");
    else
      ps.clear();
    std::vector<std::string> c = commandToVector(ps.empty() ? "" : ps[0].getValue().c_str());
    args.insert(args.end(), c.begin(), c.end());
    args.push_back("-onelab");
    args.push_back("GetDP");
    GetDP(args, onelab::server::instance());
  } while(action == "compute" && !onelabStop && (onelabUtils::incrementLoop("3") ||
                                                 onelabUtils::incrementLoop("2") ||
                                                 onelabUtils::incrementLoop("1")));
  onelabStop = false;
  locked = false;
  return onelab::server::instance()->getChanged();
}

int number_of_animation()
{
  int ret = 0;
  for(unsigned int i = 0; i < PView::list.size(); i++){
    PView * p = PView::list[i];
    if(p->getOptions()->visible){
      int numSteps = (int)p->getData()->getNumTimeSteps();
      if(numSteps > ret) ret = numSteps;
    }
  }
  return ret;
}

void set_animation(int step)
{
  for(unsigned int i = 0; i < PView::list.size(); i++){
    PView * p = PView::list[i];
    if(p->getOptions()->visible){
      p->getOptions()->timeStep = step;
      p->setChanged(true);
    }
  }
}

int animation_next()
{
  int ret = 0;
  for(unsigned int i = 0; i < PView::list.size(); i++){
    PView * p = PView::list[i];
    if(p->getOptions()->visible){
      int step = (int)p->getOptions()->timeStep + 1;
      int numSteps = (int)p->getData()->getNumTimeSteps();
      if(step < 0) step = numSteps - 1;
      if(step > numSteps - 1) step = 0;
      p->getOptions()->timeStep = step;
      p->setChanged(true);
      ret = step;
    }
  }
  return ret;
}

int animation_prev()
{
  int ret = 0;
  for(unsigned int i = 0; i < PView::list.size(); i++){
    PView * p = PView::list[i];
    if(p->getOptions()->visible){
      // skip empty steps
      int step = (int)p->getOptions()->timeStep - 1;
      int numSteps = (int)p->getData()->getNumTimeSteps();
      if(step < 0) step = numSteps - 1;
      if(step > numSteps - 1) step = 0;
      p->getOptions()->timeStep = step;
      p->setChanged(true);
      ret = step;
    }
  }
  return ret;
}
