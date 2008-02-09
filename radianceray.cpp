  //
  // C++ Implementation: radianceray
  //
  // Description:
  //
  //
  // Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2007
  //
  // Copyright: See COPYING file that comes with this distribution
  //
  //
  #include "radianceray.h"

  Vector3D refractRay(const Vector3D& e, const Vector3D& n, double nFrom, double nTo) {
    float ne = e * n;
    float reflection = 1.0 - pow((nFrom/nTo), 2.0) * (1.0 - pow(ne, 2.0));

    if (reflection < 0.0) {
            Vector3D vpar( ne * n );
            Vector3D reflDir(e - ( 2 * vpar ));
            reflDir.normalize();  
            return reflDir;
    }
    return Vector3D(((e - (n * ne)) * (nFrom/nTo) - (n * sqrt(reflection))).normal());
  }

  RadianceRay::~RadianceRay() {}

    void RadianceRay::shade(RGBvalue& result, unsigned int depth) {
        if (closestIntersection.triangle != 0 ) {
//           #ifndef NDEBUG
//               const float v = fminf((float)hittestcount/hmax, 1.0) * M_PI_2;
//               result = RGBvalue(sin(v), 0.0, 0.0);
//               return;
//           #endif
          const Triangle &hitTriangle = * ( closestIntersection.triangle );
          Vector3D n ( hitTriangle.getNormalAt ( closestIntersection ) );
          const PhongMaterial& mat = hitTriangle.getMaterial();
          const std::vector<Light> lights = scene.getLights();
          std::vector<Light>::const_iterator it;
          IntersectionResult doesntMatter;
          float tmax;
          RGBvalue direct;
          for ( it = lights.begin(); it!=lights.end(); ++it ) {
            const Light& light = *it;
            Vector3D l ( light.getPosition() -  closestIntersection.intersectionPoint );
            tmax = l.normalizeRL();
            
            float dif = n * l;
            float cspot = fmaxf(((l * -1.0f) * light.getDirection()), 0.0f);
            dif *= cspot;
            if ( dif > 0.0 ) {
              Ray intersectToLigth ( closestIntersection.intersectionPoint, l, tmax , 0.0f, &hitTriangle );
              if ( !scene.getGeometry().isBlocked(intersectToLigth) ) { //shadowtest
                direct.add ( dif * mat.diffuse[0] * light.getColor().getRGB() [0],
                            dif * mat.diffuse[1] * light.getColor().getRGB() [1],
                            dif * mat.diffuse[2] * light.getColor().getRGB() [2] );
              }
            }
          }

//           float rgb[3];
//           scene.photonMap->irradiance_estimate(rgb, closestIntersection.intersectionPoint.value, closestIntersection.triangle->getNormalAt(closestIntersection).value, 0.1, 500);
//           direct.add(rgb[0] * mat.diffuse[0], rgb[1] * mat.diffuse[1], rgb[2] * mat.diffuse[2]);

          if (depth > 0 && mat.reflection > 0.0) {
            RadianceRay reflectedRay( closestIntersection.intersectionPoint, direction.reflect(n), scene);
            reflectedRay.setIgnore(&hitTriangle);
            RGBvalue reflected;
            reflected = scene.getGeometry().trace( reflectedRay, depth - 1);
            result = RGBvalue::mix(reflected, direct, mat.reflection);
          }
          else
            result = direct;
          if ( depth > 0 && mat.alpha < 1.0 ) {
            Vector3D refrDirIn = refractRay(direction, n, 1.0, mat.refract);
            RadianceRay reflectedRay( closestIntersection.intersectionPoint, refrDirIn, scene);
            reflectedRay.setIgnore(closestIntersection.triangle);

            const Intersection &inters = scene.getGeometry().getClosestIntersection( reflectedRay );

            if ( inters.triangle != 0 ) {
              Vector3D refrDirOut = refractRay(refrDirIn, inters.triangle->getNormalAt( inters ) * -1.0, mat.refract, 1.0);

              RadianceRay r2 = RadianceRay(
                inters.intersectionPoint, 
                refrDirOut, scene
              );
              r2.setIgnore(inters.triangle );

              RGBvalue refracted = scene.getGeometry().trace(r2, depth - 1 );
              result = RGBvalue::mix(result, refracted, mat.alpha);
            }
          }
        }
      }

