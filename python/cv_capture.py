# import dependencies
import cv2
  
# create the video capture object (USB capture)
capture = cv2.VideoCapture(0)

# create the video capture object (RTSP stream)
#capture = cv2.VideoCapture("rtsp://wowzaec2demo.streamlock.net/vod/mp4:BigBuckBunny_115k.mp4")

# begin polling loop
while(True):

    # capture the video frame
    success, frame = capture.read()
    
    # check to see if we received a valid frame
    if success:

        edges = cv2.Canny(frame, 100, 200)

        # Display the resulting frame
        cv2.imshow('frame', frame)
        cv2.imshow('edges', edges)

        # check for 'q' key press
        key = cv2.waitKey(1)
        print(key)
        if key == ord('q'):
            print("terminating program")
            break
    else:
        print("WARNING: unable to retrieve frame")
  
# shutdown the capture stream and display windows
capture.release()
cv2.destroyAllWindows()