<br/>
<br/>
<br/>


<p align="center">
  <img src="https://cdn-icons-png.flaticon.com/512/8072/8072854.png" width="100" height="100">
</p>
<h1 align="center">
    <a href="https://github.com/Piero24/Food-recognition-and-leftover-estimation">Food Recognition and Leftover Estimation</a>
</h1>
<p align="center">
    <a href="https://github.com/Piero24/Food-recognition-and-leftover-estimation/commits/master">
    <img src="https://img.shields.io/github/last-commit/piero24/Food-recognition-and-leftover-estimation">
    </a>
    <a href="https://github.com/Piero24/Food-recognition-and-leftover-estimation">
    <img src="https://img.shields.io/badge/Maintained-yes-green.svg">
    </a>
    <a href="https://github.com/Piero24/twitch-stream-viewer/issues">
    <img src="https://img.shields.io/github/issues/piero24/Food-recognition-and-leftover-estimation">
    </a>
    <a href="https://github.com/Piero24/Food-recognition-and-leftover-estimation/blob/master/LICENSE">
    <img src="https://img.shields.io/github/license/piero24/Food-recognition-and-leftover-estimation">
    </a>
</p>
<p align="center">
    An awesome leftover estimator!
    <br/>
    <a href="https://github.com/Piero24/Food-recognition-and-leftover-estimation/blob/main/CV_22_23-Summer%20project.pdf"><strong> Task </strong></a>
    •
    <a href="https://github.com/Piero24/Food-recognition-and-leftover-estimation/blob/main/CV_22_23-Summer%20project.pdf"><strong> Final Paper </strong></a>
    <br/>
</p>

<br/>
<h2 id="introduction">📔  Introduction</h2>
<p>
    The goal of this project is to develop a computer vision system capable of scanning a canteen consumer's food tray at the end of a meal to estimate the amount of leftovers for each type of food. Such a system must be able to analyze pairs of images: one of the tray before the meal and one of the tray at the end of the meal. From the first image, the system will recognize the various types of food ordered, keeping track of the initial quantity of each food; at the end of the meal, the system must analyze a new image of the tray, recognizing which types of food are still present and in what quantity.

</p>
<br/>
<br/>
<p align="center">
    <img src="https://github.com/Piero24/Food-recognition-and-leftover-estimation/blob/main/LaTex/Images/IntroPhoto.png" width="600" >
</p>
<br/>
<strong>NOTE: As indicated by the professor, the trays used represent the entire available world and no limitations have been imposed on temporal and spatial complexity</strong>
<br/>

The code structure for identifying food and estimating leftovers is quite simple and it is mainly divided into 4 macro sections that are:

1. ### Food Localization

2. ### Food Segmentation

3. ### Food Classification

4. ### Results and Performance metrics

<br/>

The fundamental idea is to load the images of each tray one at a time and commence with Food Localization, which will be responsible for cleaning the image from objects or parts that are irrelevant or do not contain food, in order to more precisely identify the boxes for each
course and/or food.

<br/>
<p align="center">
    <img src="https://github.com/Piero24/Food-recognition-and-leftover-estimation/blob/main/LaTex/Images/ImagePreprocessed_tray_1.1.jpg"  width="600" >
</p>
<br/>

Once the bounding boxes in the image have been detected, they are passed on to the next section,
which will take care of refining and perfecting them through food segmentation, preparing them for classification. This operation also has a "side effect" that further improves the bounding boxes found in the previous section.

At this point, a vector containing each dish present in the image will be returned. 
After completing this step, the vector containing the outlines of the foods found in the image
is passed to the classification section. Herare based on relies on the colors, shapes, and brightness of the image to identify the types of food. 

At the same time, at each iteration in each image, the bounding boxes will be drawn for each food with the relative label. Which will be saved and merged with the other tray images at the end to show the final result.

Here you can find the result returned from some of the trays used for testing the project.

## Example of final results 

<table align="center" border="1">
    <tr>
        <td align="center"><h3><strong> Tray 1 </strong></h3></td>
        <td align="center"><h3><strong> Tray 2 </strong></h3></td>
    </tr>
    <tr>
        <td align="center"><img src="https://github.com/Piero24/Food-recognition-and-leftover-estimation/blob/main/Results/Complete%20Tray%201.jpg"></td>
        <td align="center"><img src="https://github.com/Piero24/Food-recognition-and-leftover-estimation/blob/main/Results/Complete%20Tray%202.jpg"></td>
    </tr>
    <tr>
        <td align="center"><h3><strong> Tray 5 </strong></h3></td>
        <td align="center"><h3><strong> Tray 8 </strong></h3></td>
    </tr>
    <tr>
        <td align="center"><img src="https://github.com/Piero24/Food-recognition-and-leftover-estimation/blob/main/Results/Complete%20Tray%205.jpg"></td>
        <td align="center"><img src="https://github.com/Piero24/Food-recognition-and-leftover-estimation/blob/main/Results/Complete%20Tray%208.jpg"></td>
    </tr>
</table>
<br/>
<h2 id="made-in"><br/>🛠  Built in</h2>
<p>
    The project was developed in C++ using the OpenCV library and employing image processing techniques learned during the Computer Vision course.

    It's important to note that the code could have been written using Deep Learning 
    approaches (Not Machine Learning), which would have significantly simplified and 
    improved food detection. However, I chose to develop the project exclusively with 
    the knowledge acquired during the course to apply what I've learned and directly 
    tackle the challenges and problems.
</p>
<br/>
<p align="center">
    C++ • <a href="https://opencv.org">OpenCV</a> • Computer Vision
</p>


<p align="right"><a href="#top">⇧</a></p>


<h2 id="how-to-start"><br/>⚙️  How to Start</h2>
<p>
    Below there is an example of how you can run the project.
</p>
<br/>

1. Clone the repo
  
```sh
git clone https://github.com/Piero24/Food-recognition-and-leftover-estimation.git
```

2. Install OpenCV. details can be found <a href="https://docs.opencv.org/4.x/d7/d9f/tutorial_linux_install.html">Here</a>

3. From terminal set up the project with CMake
  
```sh
cmake 
```

4. Compile the project
  
```sh
make 
```

5. Start the project
  
```sh
./Food-Recognition ./Food-recognition-and-leftover-estimation/dataset
```
Where `./Food-recognition-and-leftover-estimation/dataset` is the path of the dataset

<p align="right"><a href="#top">⇧</a></p>


---
  

<h2 id="roadmap"><br/><br/>🛫  Roadmap</h2>

- [x] Food Localization
- [x] Food Segmentation
- [x] Split the dish with different foods
- [x] Food Classification
- [ ] Implement the Leftover Estimation
- [ ] Performance measurement
- [x] Save the output for comparison

<p>
    See the 
    <a href="https://github.com/Piero24/Food-recognition-and-leftover-estimation/issues">open issues</a> for a full list of proposed features (and known issues).
</p>

<p align="right"><a href="#top">⇧</a></p>


<h3 id="responsible-disclosure"><br/>📮  Responsible Disclosure</h3>
<p>
    We assume no responsibility for an improper use of this code and everything related to it. We do not assume any responsibility for damage caused to people and / or objects in the use of the code.
</p>
<strong>
    By using this code even in a small part, the developers are declined from any responsibility.
</strong>
<br/>
<br/>
<p>
    It is possible to have more information by viewing the following links: 
    <a href="#code-of-conduct"><strong>Code of conduct</strong></a>
     • 
    <a href="#license"><strong>License</strong></a>
</p>

<p align="right"><a href="#top">⇧</a></p>


 --- 

<h2 id="license"><br/>🔍  License</h2>
<strong>MIT LICENSE</strong>
<br/>
<i>Copyright (c) 2023 Andrea Pietrobon</i>
<br/>
<br/>
<i>Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction.</i>
<br/>
<br/>
<i>Preamble</i>
<br/>
<i>The GNU General Public License is a free, copyleft license for software and other kinds of works.</i>
<br/>
<a href="https://github.com/Piero24/Food-recognition-and-leftover-estimation/blob/main/LICENSE"><strong>License Documentation »</strong></a>
<br/>
<br/>


<h3 id="authors-and-copyright"><br/>✏️  Authors and Copyright</h3>
<br/>
<p>
    👨🏽‍💻: <strong>Pietrobon Andrea</strong>
    <br/>
    🌐: <a href="https://www.pietrobonandrea.com">pietrobonandrea.com</a>
    <br/>
    <img src="https://assets.stickpng.com/thumbs/580b57fcd9996e24bc43c53e.png" width="30" height="30" align="center">:
    <a href="https://twitter.com/pietrobonandrea">@PietrobonAndrea</a>
    <br/>
    🗄: <a href="https://github.com/Piero24/Food-recognition-and-leftover-estimation">Food Recognition and Leftover Estimation</a>
</p>
<br/>
<p>
    My name is <strong>Pietrobon Andrea</strong>, a computer engineering student at the 
    <img src="https://upload.wikimedia.org/wikipedia/it/thumb/5/53/Logo_Università_Padova.svg/800px-Logo_Università_Padova.svg.png"  width="26" height="26" align="center"> 
    University of Padua (🇮🇹).
</p>
<p>
    My passion turns towards <strong>AI</strong> and <strong>ML</strong>.
    I have learned and worked in different sectors that have allowed me to gain skills in different fields, such as IT and industrial design.
    To find out more, visit my <a href="https://www.pietrobonandrea.com">
    <strong>website »</strong></a>
</p>

<p align="right"><a href="#top">⇧</a></p>



















---
> *<p align="center"> Copyright (C) by Pietrobon Andrea <br/> Released date: Jul-21-23*
