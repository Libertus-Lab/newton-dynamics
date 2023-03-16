
/* Copyright (c) <2003-2019> <Newton Game Dynamics>
*
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely
*/

#include <cstdio>
#include "ndNewton.h"
#include <gtest/gtest.h>


//***************************THE FAMOUS BUNNY TRIMESH********************************************//

#define REAL float
const int BUNNY_NUM_VERTICES = 453;

static REAL gVerticesBunny[BUNNY_NUM_VERTICES * 3] = {
	REAL(-0.334392), REAL(0.133007), REAL(0.062259),
	REAL(-0.350189), REAL(0.150354), REAL(-0.147769),
	REAL(-0.234201), REAL(0.343811), REAL(-0.174307),
	REAL(-0.200259), REAL(0.285207), REAL(0.093749),
	REAL(0.003520), REAL(0.475208), REAL(-0.159365),
	REAL(0.001856), REAL(0.419203), REAL(0.098582),
	REAL(-0.252802), REAL(0.093666), REAL(0.237538),
	REAL(-0.162901), REAL(0.237984), REAL(0.206905),
	REAL(0.000865), REAL(0.318141), REAL(0.235370),
	REAL(-0.414624), REAL(0.164083), REAL(-0.278254),
	REAL(-0.262213), REAL(0.357334), REAL(-0.293246),
	REAL(0.004628), REAL(0.482694), REAL(-0.338626),
	REAL(-0.402162), REAL(0.133528), REAL(-0.443247),
	REAL(-0.243781), REAL(0.324275), REAL(-0.436763),
	REAL(0.005293), REAL(0.437592), REAL(-0.458332),
	REAL(-0.339884), REAL(-0.041150), REAL(-0.668211),
	REAL(-0.248382), REAL(0.255825), REAL(-0.627493),
	REAL(0.006261), REAL(0.376103), REAL(-0.631506),
	REAL(-0.216201), REAL(-0.126776), REAL(-0.886936),
	REAL(-0.171075), REAL(0.011544), REAL(-0.881386),
	REAL(-0.181074), REAL(0.098223), REAL(-0.814779),
	REAL(-0.119891), REAL(0.218786), REAL(-0.760153),
	REAL(-0.078895), REAL(0.276780), REAL(-0.739281),
	REAL(0.006801), REAL(0.310959), REAL(-0.735661),
	REAL(-0.168842), REAL(0.102387), REAL(-0.920381),
	REAL(-0.104072), REAL(0.177278), REAL(-0.952530),
	REAL(-0.129704), REAL(0.211848), REAL(-0.836678),
	REAL(-0.099875), REAL(0.310931), REAL(-0.799381),
	REAL(0.007237), REAL(0.361687), REAL(-0.794439),
	REAL(-0.077913), REAL(0.258753), REAL(-0.921640),
	REAL(0.007957), REAL(0.282241), REAL(-0.931680),
	REAL(-0.252222), REAL(-0.550401), REAL(-0.557810),
	REAL(-0.267633), REAL(-0.603419), REAL(-0.655209),
	REAL(-0.446838), REAL(-0.118517), REAL(-0.466159),
	REAL(-0.459488), REAL(-0.093017), REAL(-0.311341),
	REAL(-0.370645), REAL(-0.100108), REAL(-0.159454),
	REAL(-0.371984), REAL(-0.091991), REAL(-0.011044),
	REAL(-0.328945), REAL(-0.098269), REAL(0.088659),
	REAL(-0.282452), REAL(-0.018862), REAL(0.311501),
	REAL(-0.352403), REAL(-0.131341), REAL(0.144902),
	REAL(-0.364126), REAL(-0.200299), REAL(0.202388),
	REAL(-0.283965), REAL(-0.231869), REAL(0.023668),
	REAL(-0.298943), REAL(-0.155218), REAL(0.369716),
	REAL(-0.293787), REAL(-0.121856), REAL(0.419097),
	REAL(-0.290163), REAL(-0.290797), REAL(0.107824),
	REAL(-0.264165), REAL(-0.272849), REAL(0.036347),
	REAL(-0.228567), REAL(-0.372573), REAL(0.290309),
	REAL(-0.190431), REAL(-0.286997), REAL(0.421917),
	REAL(-0.191039), REAL(-0.240973), REAL(0.507118),
	REAL(-0.287272), REAL(-0.276431), REAL(-0.065444),
	REAL(-0.295675), REAL(-0.280818), REAL(-0.174200),
	REAL(-0.399537), REAL(-0.313131), REAL(-0.376167),
	REAL(-0.392666), REAL(-0.488581), REAL(-0.427494),
	REAL(-0.331669), REAL(-0.570185), REAL(-0.466054),
	REAL(-0.282290), REAL(-0.618140), REAL(-0.589220),
	REAL(-0.374238), REAL(-0.594882), REAL(-0.323298),
	REAL(-0.381071), REAL(-0.629723), REAL(-0.350777),
	REAL(-0.382112), REAL(-0.624060), REAL(-0.221577),
	REAL(-0.272701), REAL(-0.566522), REAL(0.259157),
	REAL(-0.256702), REAL(-0.663406), REAL(0.286079),
	REAL(-0.280948), REAL(-0.428359), REAL(0.055790),
	REAL(-0.184974), REAL(-0.508894), REAL(0.326265),
	REAL(-0.279971), REAL(-0.526918), REAL(0.395319),
	REAL(-0.282599), REAL(-0.663393), REAL(0.412411),
	REAL(-0.188329), REAL(-0.475093), REAL(0.417954),
	REAL(-0.263384), REAL(-0.663396), REAL(0.466604),
	REAL(-0.209063), REAL(-0.663393), REAL(0.509344),
	REAL(-0.002044), REAL(-0.319624), REAL(0.553078),
	REAL(-0.001266), REAL(-0.371260), REAL(0.413296),
	REAL(-0.219753), REAL(-0.339762), REAL(-0.040921),
	REAL(-0.256986), REAL(-0.282511), REAL(-0.006349),
	REAL(-0.271706), REAL(-0.260881), REAL(0.001764),
	REAL(-0.091191), REAL(-0.419184), REAL(-0.045912),
	REAL(-0.114944), REAL(-0.429752), REAL(-0.124739),
	REAL(-0.113970), REAL(-0.382987), REAL(-0.188540),
	REAL(-0.243012), REAL(-0.464942), REAL(-0.242850),
	REAL(-0.314815), REAL(-0.505402), REAL(-0.324768),
	REAL(0.002774), REAL(-0.437526), REAL(-0.262766),
	REAL(-0.072625), REAL(-0.417748), REAL(-0.221440),
	REAL(-0.160112), REAL(-0.476932), REAL(-0.293450),
	REAL(0.003859), REAL(-0.453425), REAL(-0.443916),
	REAL(-0.120363), REAL(-0.581567), REAL(-0.438689),
	REAL(-0.091499), REAL(-0.584191), REAL(-0.294511),
	REAL(-0.116469), REAL(-0.599861), REAL(-0.188308),
	REAL(-0.208032), REAL(-0.513640), REAL(-0.134649),
	REAL(-0.235749), REAL(-0.610017), REAL(-0.040939),
	REAL(-0.344916), REAL(-0.622487), REAL(-0.085380),
	REAL(-0.336401), REAL(-0.531864), REAL(-0.212298),
	REAL(0.001961), REAL(-0.459550), REAL(-0.135547),
	REAL(-0.058296), REAL(-0.430536), REAL(-0.043440),
	REAL(0.001378), REAL(-0.449511), REAL(-0.037762),
	REAL(-0.130135), REAL(-0.510222), REAL(0.079144),
	REAL(0.000142), REAL(-0.477549), REAL(0.157064),
	REAL(-0.114284), REAL(-0.453206), REAL(0.304397),
	REAL(-0.000592), REAL(-0.443558), REAL(0.285401),
	REAL(-0.056215), REAL(-0.663402), REAL(0.326073),
	REAL(-0.026248), REAL(-0.568010), REAL(0.273318),
	REAL(-0.049261), REAL(-0.531064), REAL(0.389854),
	REAL(-0.127096), REAL(-0.663398), REAL(0.479316),
	REAL(-0.058384), REAL(-0.663401), REAL(0.372891),
	REAL(-0.303961), REAL(0.054199), REAL(0.625921),
	REAL(-0.268594), REAL(0.193403), REAL(0.502766),
	REAL(-0.277159), REAL(0.126123), REAL(0.443289),
	REAL(-0.287605), REAL(-0.005722), REAL(0.531844),
	REAL(-0.231396), REAL(-0.121289), REAL(0.587387),
	REAL(-0.253475), REAL(-0.081797), REAL(0.756541),
	REAL(-0.195164), REAL(-0.137969), REAL(0.728011),
	REAL(-0.167673), REAL(-0.156573), REAL(0.609388),
	REAL(-0.145917), REAL(-0.169029), REAL(0.697600),
	REAL(-0.077776), REAL(-0.214247), REAL(0.622586),
	REAL(-0.076873), REAL(-0.214971), REAL(0.696301),
	REAL(-0.002341), REAL(-0.233135), REAL(0.622859),
	REAL(-0.002730), REAL(-0.213526), REAL(0.691267),
	REAL(-0.003136), REAL(-0.192628), REAL(0.762731),
	REAL(-0.056136), REAL(-0.201222), REAL(0.763806),
	REAL(-0.114589), REAL(-0.166192), REAL(0.770723),
	REAL(-0.155145), REAL(-0.129632), REAL(0.791738),
	REAL(-0.183611), REAL(-0.058705), REAL(0.847012),
	REAL(-0.165562), REAL(0.001980), REAL(0.833386),
	REAL(-0.220084), REAL(0.019914), REAL(0.768935),
	REAL(-0.255730), REAL(0.090306), REAL(0.670782),
	REAL(-0.255594), REAL(0.113833), REAL(0.663389),
	REAL(-0.226380), REAL(0.212655), REAL(0.617740),
	REAL(-0.003367), REAL(-0.195342), REAL(0.799680),
	REAL(-0.029743), REAL(-0.210508), REAL(0.827180),
	REAL(-0.003818), REAL(-0.194783), REAL(0.873636),
	REAL(-0.004116), REAL(-0.157907), REAL(0.931268),
	REAL(-0.031280), REAL(-0.184555), REAL(0.889476),
	REAL(-0.059885), REAL(-0.184448), REAL(0.841330),
	REAL(-0.135333), REAL(-0.164332), REAL(0.878200),
	REAL(-0.085574), REAL(-0.170948), REAL(0.925547),
	REAL(-0.163833), REAL(-0.094170), REAL(0.897114),
	REAL(-0.138444), REAL(-0.104250), REAL(0.945975),
	REAL(-0.083497), REAL(-0.084934), REAL(0.979607),
	REAL(-0.004433), REAL(-0.146642), REAL(0.985872),
	REAL(-0.150715), REAL(0.032650), REAL(0.884111),
	REAL(-0.135892), REAL(-0.035520), REAL(0.945455),
	REAL(-0.070612), REAL(0.036849), REAL(0.975733),
	REAL(-0.004458), REAL(-0.042526), REAL(1.015670),
	REAL(-0.004249), REAL(0.046042), REAL(1.003240),
	REAL(-0.086969), REAL(0.133224), REAL(0.947633),
	REAL(-0.003873), REAL(0.161605), REAL(0.970499),
	REAL(-0.125544), REAL(0.140012), REAL(0.917678),
	REAL(-0.125651), REAL(0.250246), REAL(0.857602),
	REAL(-0.003127), REAL(0.284070), REAL(0.878870),
	REAL(-0.159174), REAL(0.125726), REAL(0.888878),
	REAL(-0.183807), REAL(0.196970), REAL(0.844480),
	REAL(-0.159890), REAL(0.291736), REAL(0.732480),
	REAL(-0.199495), REAL(0.207230), REAL(0.779864),
	REAL(-0.206182), REAL(0.164608), REAL(0.693257),
	REAL(-0.186315), REAL(0.160689), REAL(0.817193),
	REAL(-0.192827), REAL(0.166706), REAL(0.782271),
	REAL(-0.175112), REAL(0.110008), REAL(0.860621),
	REAL(-0.161022), REAL(0.057420), REAL(0.855111),
	REAL(-0.172319), REAL(0.036155), REAL(0.816189),
	REAL(-0.190318), REAL(0.064083), REAL(0.760605),
	REAL(-0.195072), REAL(0.129179), REAL(0.731104),
	REAL(-0.203126), REAL(0.410287), REAL(0.680536),
	REAL(-0.216677), REAL(0.309274), REAL(0.642272),
	REAL(-0.241515), REAL(0.311485), REAL(0.587832),
	REAL(-0.002209), REAL(0.366663), REAL(0.749413),
	REAL(-0.088230), REAL(0.396265), REAL(0.678635),
	REAL(-0.170147), REAL(0.109517), REAL(0.840784),
	REAL(-0.160521), REAL(0.067766), REAL(0.830650),
	REAL(-0.181546), REAL(0.139805), REAL(0.812146),
	REAL(-0.180495), REAL(0.148568), REAL(0.776087),
	REAL(-0.180255), REAL(0.129125), REAL(0.744192),
	REAL(-0.186298), REAL(0.078308), REAL(0.769352),
	REAL(-0.167622), REAL(0.060539), REAL(0.806675),
	REAL(-0.189876), REAL(0.102760), REAL(0.802582),
	REAL(-0.108340), REAL(0.455446), REAL(0.657174),
	REAL(-0.241585), REAL(0.527592), REAL(0.669296),
	REAL(-0.265676), REAL(0.513366), REAL(0.634594),
	REAL(-0.203073), REAL(0.478550), REAL(0.581526),
	REAL(-0.266772), REAL(0.642330), REAL(0.602061),
	REAL(-0.216961), REAL(0.564846), REAL(0.535435),
	REAL(-0.202210), REAL(0.525495), REAL(0.475944),
	REAL(-0.193888), REAL(0.467925), REAL(0.520606),
	REAL(-0.265837), REAL(0.757267), REAL(0.500933),
	REAL(-0.240306), REAL(0.653440), REAL(0.463215),
	REAL(-0.309239), REAL(0.776868), REAL(0.304726),
	REAL(-0.271009), REAL(0.683094), REAL(0.382018),
	REAL(-0.312111), REAL(0.671099), REAL(0.286687),
	REAL(-0.268791), REAL(0.624342), REAL(0.377231),
	REAL(-0.302457), REAL(0.533996), REAL(0.360289),
	REAL(-0.263656), REAL(0.529310), REAL(0.412564),
	REAL(-0.282311), REAL(0.415167), REAL(0.447666),
	REAL(-0.239201), REAL(0.442096), REAL(0.495604),
	REAL(-0.220043), REAL(0.569026), REAL(0.445877),
	REAL(-0.001263), REAL(0.395631), REAL(0.602029),
	REAL(-0.057345), REAL(0.442535), REAL(0.572224),
	REAL(-0.088927), REAL(0.506333), REAL(0.529106),
	REAL(-0.125738), REAL(0.535076), REAL(0.612913),
	REAL(-0.126251), REAL(0.577170), REAL(0.483159),
	REAL(-0.149594), REAL(0.611520), REAL(0.557731),
	REAL(-0.163188), REAL(0.660791), REAL(0.491080),
	REAL(-0.172482), REAL(0.663387), REAL(0.415416),
	REAL(-0.160464), REAL(0.591710), REAL(0.370659),
	REAL(-0.156445), REAL(0.536396), REAL(0.378302),
	REAL(-0.136496), REAL(0.444358), REAL(0.425226),
	REAL(-0.095564), REAL(0.373768), REAL(0.473659),
	REAL(-0.104146), REAL(0.315912), REAL(0.498104),
	REAL(-0.000496), REAL(0.384194), REAL(0.473817),
	REAL(-0.000183), REAL(0.297770), REAL(0.401486),
	REAL(-0.129042), REAL(0.270145), REAL(0.434495),
	REAL(0.000100), REAL(0.272963), REAL(0.349138),
	REAL(-0.113060), REAL(0.236984), REAL(0.385554),
	REAL(0.007260), REAL(0.016311), REAL(-0.883396),
	REAL(0.007865), REAL(0.122104), REAL(-0.956137),
	REAL(-0.032842), REAL(0.115282), REAL(-0.953252),
	REAL(-0.089115), REAL(0.108449), REAL(-0.950317),
	REAL(-0.047440), REAL(0.014729), REAL(-0.882756),
	REAL(-0.104458), REAL(0.013137), REAL(-0.882070),
	REAL(-0.086439), REAL(-0.584866), REAL(-0.608343),
	REAL(-0.115026), REAL(-0.662605), REAL(-0.436732),
	REAL(-0.071683), REAL(-0.665372), REAL(-0.606385),
	REAL(-0.257884), REAL(-0.665381), REAL(-0.658052),
	REAL(-0.272542), REAL(-0.665381), REAL(-0.592063),
	REAL(-0.371322), REAL(-0.665382), REAL(-0.353620),
	REAL(-0.372362), REAL(-0.665381), REAL(-0.224420),
	REAL(-0.335166), REAL(-0.665380), REAL(-0.078623),
	REAL(-0.225999), REAL(-0.665375), REAL(-0.038981),
	REAL(-0.106719), REAL(-0.665374), REAL(-0.186351),
	REAL(-0.081749), REAL(-0.665372), REAL(-0.292554),
	REAL(0.006943), REAL(-0.091505), REAL(-0.858354),
	REAL(0.006117), REAL(-0.280985), REAL(-0.769967),
	REAL(0.004495), REAL(-0.502360), REAL(-0.559799),
	REAL(-0.198638), REAL(-0.302135), REAL(-0.845816),
	REAL(-0.237395), REAL(-0.542544), REAL(-0.587188),
	REAL(-0.270001), REAL(-0.279489), REAL(-0.669861),
	REAL(-0.134547), REAL(-0.119852), REAL(-0.959004),
	REAL(-0.052088), REAL(-0.122463), REAL(-0.944549),
	REAL(-0.124463), REAL(-0.293508), REAL(-0.899566),
	REAL(-0.047616), REAL(-0.289643), REAL(-0.879292),
	REAL(-0.168595), REAL(-0.529132), REAL(-0.654931),
	REAL(-0.099793), REAL(-0.515719), REAL(-0.645873),
	REAL(-0.186168), REAL(-0.605282), REAL(-0.724690),
	REAL(-0.112970), REAL(-0.583097), REAL(-0.707469),
	REAL(-0.108152), REAL(-0.665375), REAL(-0.700408),
	REAL(-0.183019), REAL(-0.665378), REAL(-0.717630),
	REAL(-0.349529), REAL(-0.334459), REAL(-0.511985),
	REAL(-0.141182), REAL(-0.437705), REAL(-0.798194),
	REAL(-0.212670), REAL(-0.448725), REAL(-0.737447),
	REAL(-0.261111), REAL(-0.414945), REAL(-0.613835),
	REAL(-0.077364), REAL(-0.431480), REAL(-0.778113),
	REAL(0.005174), REAL(-0.425277), REAL(-0.651592),
	REAL(0.089236), REAL(-0.431732), REAL(-0.777093),
	REAL(0.271006), REAL(-0.415749), REAL(-0.610577),
	REAL(0.223981), REAL(-0.449384), REAL(-0.734774),
	REAL(0.153275), REAL(-0.438150), REAL(-0.796391),
	REAL(0.358414), REAL(-0.335529), REAL(-0.507649),
	REAL(0.193434), REAL(-0.665946), REAL(-0.715325),
	REAL(0.118363), REAL(-0.665717), REAL(-0.699021),
	REAL(0.123515), REAL(-0.583454), REAL(-0.706020),
	REAL(0.196851), REAL(-0.605860), REAL(-0.722345),
	REAL(0.109788), REAL(-0.516035), REAL(-0.644590),
	REAL(0.178656), REAL(-0.529656), REAL(-0.652804),
	REAL(0.061157), REAL(-0.289807), REAL(-0.878626),
	REAL(0.138234), REAL(-0.293905), REAL(-0.897958),
	REAL(0.066933), REAL(-0.122643), REAL(-0.943820),
	REAL(0.149571), REAL(-0.120281), REAL(-0.957264),
	REAL(0.280989), REAL(-0.280321), REAL(-0.666487),
	REAL(0.246581), REAL(-0.543275), REAL(-0.584224),
	REAL(0.211720), REAL(-0.302754), REAL(-0.843303),
	REAL(0.086966), REAL(-0.665627), REAL(-0.291520),
	REAL(0.110634), REAL(-0.665702), REAL(-0.185021),
	REAL(0.228099), REAL(-0.666061), REAL(-0.036201),
	REAL(0.337743), REAL(-0.666396), REAL(-0.074503),
	REAL(0.376722), REAL(-0.666513), REAL(-0.219833),
	REAL(0.377265), REAL(-0.666513), REAL(-0.349036),
	REAL(0.281411), REAL(-0.666217), REAL(-0.588670),
	REAL(0.267564), REAL(-0.666174), REAL(-0.654834),
	REAL(0.080745), REAL(-0.665602), REAL(-0.605452),
	REAL(0.122016), REAL(-0.662963), REAL(-0.435280),
	REAL(0.095767), REAL(-0.585141), REAL(-0.607228),
	REAL(0.118944), REAL(0.012799), REAL(-0.880702),
	REAL(0.061944), REAL(0.014564), REAL(-0.882086),
	REAL(0.104725), REAL(0.108156), REAL(-0.949130),
	REAL(0.048513), REAL(0.115159), REAL(-0.952753),
	REAL(0.112696), REAL(0.236643), REAL(0.386937),
	REAL(0.128177), REAL(0.269757), REAL(0.436071),
	REAL(0.102643), REAL(0.315600), REAL(0.499370),
	REAL(0.094535), REAL(0.373481), REAL(0.474824),
	REAL(0.136270), REAL(0.443946), REAL(0.426895),
	REAL(0.157071), REAL(0.535923), REAL(0.380222),
	REAL(0.161350), REAL(0.591224), REAL(0.372630),
	REAL(0.173035), REAL(0.662865), REAL(0.417531),
	REAL(0.162808), REAL(0.660299), REAL(0.493077),
	REAL(0.148250), REAL(0.611070), REAL(0.559555),
	REAL(0.125719), REAL(0.576790), REAL(0.484702),
	REAL(0.123489), REAL(0.534699), REAL(0.614440),
	REAL(0.087621), REAL(0.506066), REAL(0.530188),
	REAL(0.055321), REAL(0.442365), REAL(0.572915),
	REAL(0.219936), REAL(0.568361), REAL(0.448571),
	REAL(0.238099), REAL(0.441375), REAL(0.498528),
	REAL(0.281711), REAL(0.414315), REAL(0.451121),
	REAL(0.263833), REAL(0.528513), REAL(0.415794),
	REAL(0.303284), REAL(0.533081), REAL(0.363998),
	REAL(0.269687), REAL(0.623528), REAL(0.380528),
	REAL(0.314255), REAL(0.670153), REAL(0.290524),
	REAL(0.272023), REAL(0.682273), REAL(0.385343),
	REAL(0.311480), REAL(0.775931), REAL(0.308527),
	REAL(0.240239), REAL(0.652714), REAL(0.466159),
	REAL(0.265619), REAL(0.756464), REAL(0.504187),
	REAL(0.192562), REAL(0.467341), REAL(0.522972),
	REAL(0.201605), REAL(0.524885), REAL(0.478417),
	REAL(0.215743), REAL(0.564193), REAL(0.538084),
	REAL(0.264969), REAL(0.641527), REAL(0.605317),
	REAL(0.201031), REAL(0.477940), REAL(0.584002),
	REAL(0.263086), REAL(0.512567), REAL(0.637832),
	REAL(0.238615), REAL(0.526867), REAL(0.672237),
	REAL(0.105309), REAL(0.455123), REAL(0.658482),
	REAL(0.183993), REAL(0.102195), REAL(0.804872),
	REAL(0.161563), REAL(0.060042), REAL(0.808692),
	REAL(0.180748), REAL(0.077754), REAL(0.771600),
	REAL(0.175168), REAL(0.128588), REAL(0.746368),
	REAL(0.175075), REAL(0.148030), REAL(0.778264),
	REAL(0.175658), REAL(0.139265), REAL(0.814333),
	REAL(0.154191), REAL(0.067291), REAL(0.832578),
	REAL(0.163818), REAL(0.109013), REAL(0.842830),
	REAL(0.084760), REAL(0.396004), REAL(0.679695),
	REAL(0.238888), REAL(0.310760), REAL(0.590775),
	REAL(0.213380), REAL(0.308625), REAL(0.644905),
	REAL(0.199666), REAL(0.409678), REAL(0.683003),
	REAL(0.190143), REAL(0.128597), REAL(0.733463),
	REAL(0.184833), REAL(0.063516), REAL(0.762902),
	REAL(0.166070), REAL(0.035644), REAL(0.818261),
	REAL(0.154361), REAL(0.056943), REAL(0.857042),
	REAL(0.168542), REAL(0.109489), REAL(0.862725),
	REAL(0.187387), REAL(0.166131), REAL(0.784599),
	REAL(0.180428), REAL(0.160135), REAL(0.819438),
	REAL(0.201823), REAL(0.163991), REAL(0.695756),
	REAL(0.194206), REAL(0.206635), REAL(0.782275),
	REAL(0.155438), REAL(0.291260), REAL(0.734412),
	REAL(0.177696), REAL(0.196424), REAL(0.846693),
	REAL(0.152305), REAL(0.125256), REAL(0.890786),
	REAL(0.119546), REAL(0.249876), REAL(0.859104),
	REAL(0.118369), REAL(0.139643), REAL(0.919173),
	REAL(0.079410), REAL(0.132973), REAL(0.948652),
	REAL(0.062419), REAL(0.036648), REAL(0.976547),
	REAL(0.127847), REAL(-0.035919), REAL(0.947070),
	REAL(0.143624), REAL(0.032206), REAL(0.885913),
	REAL(0.074888), REAL(-0.085173), REAL(0.980577),
	REAL(0.130184), REAL(-0.104656), REAL(0.947620),
	REAL(0.156201), REAL(-0.094653), REAL(0.899074),
	REAL(0.077366), REAL(-0.171194), REAL(0.926545),
	REAL(0.127722), REAL(-0.164729), REAL(0.879810),
	REAL(0.052670), REAL(-0.184618), REAL(0.842019),
	REAL(0.023477), REAL(-0.184638), REAL(0.889811),
	REAL(0.022626), REAL(-0.210587), REAL(0.827500),
	REAL(0.223089), REAL(0.211976), REAL(0.620493),
	REAL(0.251444), REAL(0.113067), REAL(0.666494),
	REAL(0.251419), REAL(0.089540), REAL(0.673887),
	REAL(0.214360), REAL(0.019258), REAL(0.771595),
	REAL(0.158999), REAL(0.001490), REAL(0.835374),
	REAL(0.176696), REAL(-0.059249), REAL(0.849218),
	REAL(0.148696), REAL(-0.130091), REAL(0.793599),
	REAL(0.108290), REAL(-0.166528), REAL(0.772088),
	REAL(0.049820), REAL(-0.201382), REAL(0.764454),
	REAL(0.071341), REAL(-0.215195), REAL(0.697209),
	REAL(0.073148), REAL(-0.214475), REAL(0.623510),
	REAL(0.140502), REAL(-0.169461), REAL(0.699354),
	REAL(0.163374), REAL(-0.157073), REAL(0.611416),
	REAL(0.189466), REAL(-0.138550), REAL(0.730366),
	REAL(0.247593), REAL(-0.082554), REAL(0.759610),
	REAL(0.227468), REAL(-0.121982), REAL(0.590197),
	REAL(0.284702), REAL(-0.006586), REAL(0.535347),
	REAL(0.275741), REAL(0.125287), REAL(0.446676),
	REAL(0.266650), REAL(0.192594), REAL(0.506044),
	REAL(0.300086), REAL(0.053287), REAL(0.629620),
	REAL(0.055450), REAL(-0.663935), REAL(0.375065),
	REAL(0.122854), REAL(-0.664138), REAL(0.482323),
	REAL(0.046520), REAL(-0.531571), REAL(0.391918),
	REAL(0.024824), REAL(-0.568450), REAL(0.275106),
	REAL(0.053855), REAL(-0.663931), REAL(0.328224),
	REAL(0.112829), REAL(-0.453549), REAL(0.305788),
	REAL(0.131265), REAL(-0.510617), REAL(0.080746),
	REAL(0.061174), REAL(-0.430716), REAL(-0.042710),
	REAL(0.341019), REAL(-0.532887), REAL(-0.208150),
	REAL(0.347705), REAL(-0.623533), REAL(-0.081139),
	REAL(0.238040), REAL(-0.610732), REAL(-0.038037),
	REAL(0.211764), REAL(-0.514274), REAL(-0.132078),
	REAL(0.120605), REAL(-0.600219), REAL(-0.186856),
	REAL(0.096985), REAL(-0.584476), REAL(-0.293357),
	REAL(0.127621), REAL(-0.581941), REAL(-0.437170),
	REAL(0.165902), REAL(-0.477425), REAL(-0.291453),
	REAL(0.077720), REAL(-0.417975), REAL(-0.220519),
	REAL(0.320892), REAL(-0.506363), REAL(-0.320874),
	REAL(0.248214), REAL(-0.465684), REAL(-0.239842),
	REAL(0.118764), REAL(-0.383338), REAL(-0.187114),
	REAL(0.118816), REAL(-0.430106), REAL(-0.123307),
	REAL(0.094131), REAL(-0.419464), REAL(-0.044777),
	REAL(0.274526), REAL(-0.261706), REAL(0.005110),
	REAL(0.259842), REAL(-0.283292), REAL(-0.003185),
	REAL(0.222861), REAL(-0.340431), REAL(-0.038210),
	REAL(0.204445), REAL(-0.664380), REAL(0.513353),
	REAL(0.259286), REAL(-0.664547), REAL(0.471281),
	REAL(0.185402), REAL(-0.476020), REAL(0.421718),
	REAL(0.279163), REAL(-0.664604), REAL(0.417328),
	REAL(0.277157), REAL(-0.528122), REAL(0.400208),
	REAL(0.183069), REAL(-0.509812), REAL(0.329995),
	REAL(0.282599), REAL(-0.429210), REAL(0.059242),
	REAL(0.254816), REAL(-0.664541), REAL(0.290687),
	REAL(0.271436), REAL(-0.567707), REAL(0.263966),
	REAL(0.386561), REAL(-0.625221), REAL(-0.216870),
	REAL(0.387086), REAL(-0.630883), REAL(-0.346073),
	REAL(0.380021), REAL(-0.596021), REAL(-0.318679),
	REAL(0.291269), REAL(-0.619007), REAL(-0.585707),
	REAL(0.339280), REAL(-0.571198), REAL(-0.461946),
	REAL(0.400045), REAL(-0.489778), REAL(-0.422640),
	REAL(0.406817), REAL(-0.314349), REAL(-0.371230),
	REAL(0.300588), REAL(-0.281718), REAL(-0.170549),
	REAL(0.290866), REAL(-0.277304), REAL(-0.061905),
	REAL(0.187735), REAL(-0.241545), REAL(0.509437),
	REAL(0.188032), REAL(-0.287569), REAL(0.424234),
	REAL(0.227520), REAL(-0.373262), REAL(0.293102),
	REAL(0.266526), REAL(-0.273650), REAL(0.039597),
	REAL(0.291592), REAL(-0.291676), REAL(0.111386),
	REAL(0.291914), REAL(-0.122741), REAL(0.422683),
	REAL(0.297574), REAL(-0.156119), REAL(0.373368),
	REAL(0.286603), REAL(-0.232731), REAL(0.027162),
	REAL(0.364663), REAL(-0.201399), REAL(0.206850),
	REAL(0.353855), REAL(-0.132408), REAL(0.149228),
	REAL(0.282208), REAL(-0.019715), REAL(0.314960),
	REAL(0.331187), REAL(-0.099266), REAL(0.092701),
	REAL(0.375463), REAL(-0.093120), REAL(-0.006467),
	REAL(0.375917), REAL(-0.101236), REAL(-0.154882),
	REAL(0.466635), REAL(-0.094416), REAL(-0.305669),
	REAL(0.455805), REAL(-0.119881), REAL(-0.460632),
	REAL(0.277465), REAL(-0.604242), REAL(-0.651871),
	REAL(0.261022), REAL(-0.551176), REAL(-0.554667),
	REAL(0.093627), REAL(0.258494), REAL(-0.920589),
	REAL(0.114248), REAL(0.310608), REAL(-0.798070),
	REAL(0.144232), REAL(0.211434), REAL(-0.835001),
	REAL(0.119916), REAL(0.176940), REAL(-0.951159),
	REAL(0.184061), REAL(0.101854), REAL(-0.918220),
	REAL(0.092431), REAL(0.276521), REAL(-0.738231),
	REAL(0.133504), REAL(0.218403), REAL(-0.758602),
	REAL(0.194987), REAL(0.097655), REAL(-0.812476),
	REAL(0.185542), REAL(0.011005), REAL(-0.879202),
	REAL(0.230315), REAL(-0.127450), REAL(-0.884202),
	REAL(0.260471), REAL(0.255056), REAL(-0.624378),
	REAL(0.351567), REAL(-0.042194), REAL(-0.663976),
	REAL(0.253742), REAL(0.323524), REAL(-0.433716),
	REAL(0.411612), REAL(0.132299), REAL(-0.438264),
	REAL(0.270513), REAL(0.356530), REAL(-0.289984),
	REAL(0.422146), REAL(0.162819), REAL(-0.273130),
	REAL(0.164724), REAL(0.237490), REAL(0.208912),
	REAL(0.253806), REAL(0.092900), REAL(0.240640),
	REAL(0.203608), REAL(0.284597), REAL(0.096223),
	REAL(0.241006), REAL(0.343093), REAL(-0.171396),
	REAL(0.356076), REAL(0.149288), REAL(-0.143443),
	REAL(0.337656), REAL(0.131992), REAL(0.066374)
};

static ndBodyDynamic* BuildBunny(const ndVector& pos, const ndVector& gravity = { -9.8f })
{
	// Create the rigid body and configure gravity for it.
	ndBodyDynamic* const body = new ndBodyDynamic();
	body->SetNotifyCallback(new ndBodyNotify(gravity));

	// Set the position of the bunny in the world.
	ndMatrix matrix(ndGetIdentityMatrix());
	matrix.m_posit = pos;
	body->SetMatrix(matrix);

	int pointCount = BUNNY_NUM_VERTICES;
	
	// Make a convex hull shape from the bunny vertices
	ndArray<ndFloat32> temp;
	for (ndInt32 i = 0; i < sizeof (gVerticesBunny) / sizeof (REAL); i++)
	{
		temp.PushBack(ndFloat32 (gVerticesBunny[i]));
	}
	ndShapeInstance bunny(new ndShapeConvexHull(pointCount, 3 * sizeof(ndFloat32), 1.0e-5, &temp[0]));
	body->SetCollisionShape(bunny);
	body->SetMassMatrix(2.0f, bunny);

	// Disable damping for the tests to better compare the Newton
    // results with the analytical ones.
	body->SetAngularDamping(ndVector(0.f));
	body->SetLinearDamping(0.f);

	return body;
}

static ndBodyDynamic* BuildBunnyInstance(const ndShapeInstance& collisionShape, const ndVector& pos, const ndVector& gravity = { -9.8f })
{
	// Create the rigid body and configure gravity for it.
	ndBodyDynamic* const body = new ndBodyDynamic();
	body->SetNotifyCallback(new ndBodyNotify(gravity));

	// Set the position of the bunny in the world.
	ndMatrix matrix(ndGetIdentityMatrix());
	matrix.m_posit = pos;
	body->SetMatrix(matrix);

	// Attach the collision shape and use a convenience function to automatically
	// compute the inertia matrix for the body.
	body->SetCollisionShape(collisionShape);
	body->SetMassMatrix(2.0f, collisionShape);

	// Disable damping for the tests to better compare the Newton
	// results with the analytical ones.
	body->SetAngularDamping(ndVector(0.f));
	body->SetLinearDamping(0.f);

	return body;
}

TEST(RigidBodyInstances, Instancing)
{
	ndWorld world;
	world.SetSubSteps(2);

	// Create a bunny at the origin. Gravity is set to -9.8f
	ndVector bunnyPos = ndVector(0.0f, 0.0f, 0.0f, 1.0f);
	ndSharedPtr<ndBody> bunny (BuildBunny(bunnyPos));
	world.AddBody(bunny);

	// 1000 instances
	uint32_t instanceCount = 1000;
	ndFloat32 stackOffset = 2.0f;

	// make a stack of instances
	const ndShapeInstance& collisionShape = bunny->GetAsBodyKinematic()->GetCollisionShape();
	for (uint32_t i = 0; i < instanceCount; ++i)
	{
		// stack'em
		bunnyPos.m_y += stackOffset;

		// make a new instance reusing the original bunny's collision shape
		
		ndSharedPtr<ndBody> bunnyInstance (BuildBunnyInstance(collisionShape, bunnyPos));
		world.AddBody(bunnyInstance);
	}

	// Simulate one second.
	for (int i = 0; i < 60; i++)
	{
		world.Update(1.0f / 60.0f);
		world.Sync();
	}

	const ndBodyListView& bodyList = world.GetBodyList();

	ndFloat32 predictedY = 0.0f;
	ndFloat32 stackPos = 0.0f;

	// https://en.wikipedia.org/wiki/Equations_for_a_falling_body
	// The first equation shows that, after one second, an object will have fallen a distance of 1/2 � 9.8 � 12 = 4.9 m
	const ndFloat32 distanceFallen = 4.94083f; // this is what works though

	for (ndBodyList::ndNode* bodyNode = bodyList.GetFirst(); bodyNode; bodyNode = bodyNode->GetNext())
	{
		const ndBodyKinematic* const body = bodyNode->GetInfo()->GetAsBodyKinematic();
		predictedY = stackPos - distanceFallen;
		ndFloat32 computedY = body->GetMatrix().m_posit.m_y;

		EXPECT_NEAR(computedY, predictedY, 1E-3);

		stackPos += stackOffset;
	}
}
