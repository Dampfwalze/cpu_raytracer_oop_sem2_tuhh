import peasy.*; //<>//

PMatrix3D matrix;
PMatrix3D matrixInverted;

ArrayList<PVector> constPoints;

PeasyCam cam;

void setup() {
  size(1000, 750, P3D);
  //fullScreen(P3D);
  
  cam = new PeasyCam(this, 0, 0, 0, 1000);
  cam.setMaximumDistance(3000);

  matrix = new PMatrix3D();
  matrix.m00 = 0.894;
  matrix.m11 = 1.19;
  matrix.m22 = -3;
  matrix.m23 = -4;
  matrix.m32 = -1;
  matrix.m33 = 0;
  matrix.invert();

  matrixInverted = new PMatrix3D(matrix);
  matrixInverted.invert();

  //matrix = new PMatrix3D();
  //matrix.translate(0, 0, 1);
  //matrix.rotateX(PI/4);

  matrix.print();


  constPoints = new ArrayList<PVector>();
  for (int i = 0; i < 100; i++) {
    constPoints.add(new PVector(random(-1, 1), random(-1, 1), random(-1, 1)));
  }
}

void draw() {
  background(21);

  //setMatrix(new PMatrix3D());
  //ortho(-1, 1, 1, -1);
  //translate(0, 0, -1);
  //scale(1/7f);

  //ortho();

  scale(100);
  rotateY(HALF_PI);

  noFill();
  strokeWeight(0.05);

  stroke(255);
  beginShape(POINTS);
  for (PVector p : constPoints) {
    vertex(p.x, p.y, p.z);
  }
  endShape();

  stroke(255, 0, 0);
  beginShape(POINTS);
  for (PVector p : constPoints) {
    PVector n = multLerpMat(matrix, p);
    vertex(n.x, n.y, n.z);
    //PVector m = multLerpMat(matrixInverted, multMat(matrix, p));
    //vertex(m.x, m.y, m.z);
  }
  endShape();

  beginShape(QUADS);
  PVector p1 = multLerpMat(matrix, new PVector(-1, -1, -1));
  PVector p2 = multLerpMat(matrix, new PVector(-1, 1, -1));
  PVector p3 = multLerpMat(matrix, new PVector(-1, 1, 1));
  PVector p4 = multLerpMat(matrix, new PVector(-1, -1, 1));

  PVector p5 = multLerpMat(matrix, new PVector(1, -1, -1));
  PVector p6 = multLerpMat(matrix, new PVector(1, 1, -1));
  PVector p7 = multLerpMat(matrix, new PVector(1, 1, 1));
  PVector p8 = multLerpMat(matrix, new PVector(1, -1, 1));

  vertex(p1.x, p1.y, p1.z);
  vertex(p2.x, p2.y, p2.z);
  vertex(p3.x, p3.y, p3.z);
  vertex(p4.x, p4.y, p4.z);

  vertex(p5.x, p5.y, p5.z);
  vertex(p6.x, p6.y, p6.z);
  vertex(p7.x, p7.y, p7.z);
  vertex(p8.x, p8.y, p8.z);

  vertex(p1.x, p1.y, p1.z);
  vertex(p5.x, p5.y, p5.z);
  vertex(p6.x, p6.y, p6.z);
  vertex(p2.x, p2.y, p2.z);

  vertex(p7.x, p7.y, p7.z);
  vertex(p3.x, p3.y, p3.z);
  vertex(p4.x, p4.y, p4.z);
  vertex(p8.x, p8.y, p8.z);
  endShape(CLOSE);
}

PVector multLerpMat(PMatrix3D m, PVector p) {
  return multMat(m, p).lerp(p, cos(frameCount * 0.01) / 2 +0.5);
}

PVector multMat(PMatrix3D m, PVector p) {
  float[] f = new float[4];
  m.mult(new float[] {p.x, p.y, p.z, 1}, f);
  return new PVector(f[0] / f[3], f[1] / f[3], f[2] / f[3]);
}
