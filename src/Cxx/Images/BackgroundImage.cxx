#include <vtkVersion.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkImageCanvasSource2D.h>
#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkJPEGReader.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkSuperquadricSource.h>

int main(int argc, char *argv[])
{
  vtkSmartPointer<vtkImageData> imageData;

  // Verify input arguments
  if ( argc > 1 )
  {
    //Read the image
    vtkSmartPointer<vtkJPEGReader> jpegReader =
      vtkSmartPointer<vtkJPEGReader>::New();
    if( !jpegReader->CanReadFile( argv[1] ) )
    {
      std::cerr << "Error reading file " << argv[1] << std::endl;
      return EXIT_FAILURE;
    }
    jpegReader->SetFileName ( argv[1] );
    jpegReader->Update();
    imageData = jpegReader->GetOutput();
  }
  else
  {
    vtkSmartPointer<vtkImageCanvasSource2D> canvasSource =
      vtkSmartPointer<vtkImageCanvasSource2D>::New();
    canvasSource->SetExtent(0, 100, 0, 100, 0, 0);
    canvasSource->SetScalarTypeToUnsignedChar();
    canvasSource->SetNumberOfScalarComponents(3);
    canvasSource->SetDrawColor(127, 127, 100);
    canvasSource->FillBox(0, 100, 0, 100);
    canvasSource->SetDrawColor(100, 255, 255);
    canvasSource->FillTriangle(10, 10, 25, 10, 25, 25);
    canvasSource->SetDrawColor(255, 100, 255);
    canvasSource->FillTube(75, 75, 0, 75, 5.0);
    canvasSource->Update();
    imageData = canvasSource->GetOutput();
  }
  // Create an image actor to display the image
  vtkSmartPointer<vtkImageActor> imageActor =
    vtkSmartPointer<vtkImageActor>::New();
#if VTK_MAJOR_VERSION <= 5
  imageActor->SetInput(imageData);
#else
  imageActor->SetInputData(imageData);
#endif

  // Create a renderer to display the image in the background
  vtkSmartPointer<vtkRenderer> backgroundRenderer =
    vtkSmartPointer<vtkRenderer>::New();

  // Create a superquadric
  vtkSmartPointer<vtkSuperquadricSource> superquadricSource =
    vtkSmartPointer<vtkSuperquadricSource>::New();
  superquadricSource->SetPhiRoundness(1.1);
  superquadricSource->SetThetaRoundness(.2);

  // Create a mapper and actor
  vtkSmartPointer<vtkPolyDataMapper> superquadricMapper =
    vtkSmartPointer<vtkPolyDataMapper>::New();
  superquadricMapper->SetInputConnection(superquadricSource->GetOutputPort());

  vtkSmartPointer<vtkActor> superquadricActor =
    vtkSmartPointer<vtkActor>::New();
  superquadricActor->SetMapper(superquadricMapper);

  vtkSmartPointer<vtkRenderer> sceneRenderer =
    vtkSmartPointer<vtkRenderer>::New();

  vtkSmartPointer<vtkRenderWindow> renderWindow =
    vtkSmartPointer<vtkRenderWindow>::New();

  // Set up the render window and renderers such that there is
  // a background layer and a foreground layer
  backgroundRenderer->SetLayer(0);
  backgroundRenderer->InteractiveOff();
  sceneRenderer->SetLayer(1);
  renderWindow->SetNumberOfLayers(2);
  renderWindow->AddRenderer(backgroundRenderer);
  renderWindow->AddRenderer(sceneRenderer);

  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add actors to the renderers
  sceneRenderer->AddActor(superquadricActor);
  backgroundRenderer->AddActor(imageActor);

  // Render once to figure out where the background camera will be
  renderWindow->Render();

  // Set up the background camera to fill the renderer with the image
  double origin[3];
  double spacing[3];
  int extent[6];
  imageData->GetOrigin( origin );
  imageData->GetSpacing( spacing );
  imageData->GetExtent( extent );

  vtkCamera* camera = backgroundRenderer->GetActiveCamera();
  camera->ParallelProjectionOn();

  double xc = origin[0] + 0.5*(extent[0] + extent[1])*spacing[0];
  double yc = origin[1] + 0.5*(extent[2] + extent[3])*spacing[1];
  //double xd = (extent[1] - extent[0] + 1)*spacing[0];
  double yd = (extent[3] - extent[2] + 1)*spacing[1];
  double d = camera->GetDistance();
  camera->SetParallelScale(0.5*yd);
  camera->SetFocalPoint(xc,yc,0.0);
  camera->SetPosition(xc,yc,d);

  // Render again to set the correct view
  renderWindow->Render();

  // Interact with the window
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
