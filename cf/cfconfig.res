R"(

{
    "version" : 2,
    "functions" : [
        {
            "name": "RegisterPositionToPlane",
            "description": "Project a point onto a plane along the normal vector of the plane.",
            "applicableFor" : [ "Point" ],

            "neededElements": [
                {
                    "name": "Position",
                    "description" : "Select one point to register to the plane.",
                    "infinite": true,
                    "typeOfElement": "Position"
                },
                {
                    "name": "Plane",
                    "description" : "Select a plane in which the target geometry is to be projected.",
                    "infinite": false,
                    "typeOfElement": "Plane"
                }
            ],

            "parameter": [
                {
                    "name" : "PointFromPoints",
                    "feature" : "global",
                    "parameter" : [
                        { "name" : "Position" }
                    ]
                },
                {
                    "name" : "Register",
                    "feature" : "global",
                    "parameter" : [
                        { "name": "Plane" }
                    ]
                }
            ]
        },
        
        {
            "name": "DistanceFromPlane",
            "description": "Calculate distance between point and plan.",
            "applicableFor" : [ "ScalarEntityDistance" ],

            "neededElements": [
                {
                    "name": "Position",
                    "description" : "Select one point to register to the plane.",
                    "infinite": true,
                    "typeOfElement": "Position"
                },
                {
                    "name": "Plane",
                    "description" : "Select a plane in which the target geometry is to be projected.",
                    "infinite": false,
                    "typeOfElement": "Plane"
                }
            ],

            "parameter": [
                {
                    "name" : "DistanceBetweenTwoPoints",
                    "feature" : "global",
                    "parameter" : [
                        { "name": "Position" },
                        {
                            "name" : "RegisterPositionToPlane",
                            "feature" : "Point",
                            "parameter" : [
                                { "name": "Position" },
                                { "name": "Plane" }
                            ]
                        }
                    ]
                }

            ]
        },

        {
            "name": "XDistance",
            "description": "Project a point onto a plane along the x axis.",
            "applicableFor" : [ "ScalarEntityDistance" ],

            "neededElements": [
                {
                    "name": "Position",
                    "description" : "Select one point to register to the plane.",
                    "infinite": true,
                    "typeOfElement": "Position"
                },
                {
                    "name": "Plane",
                    "description" : "Select a plane in which the target geometry is to be projected.",
                    "infinite": false,
                    "typeOfElement": "Plane"
                }
            ],

            "parameter": [
                {
                    "name" : "DistanceBetweenTwoPoints",
                    "feature": "global",
                    "parameter" : [
                        {
                            "name" : "YZFilter",
                            "feature": "Point",
                            "comment" : "x-Achse",
                            "parameter" : [
                                { "name": "Position" }
                            ]
                        },
                        {
                            "name" : "YZFilter",
                            "feature": "Point",
                            "comment" : "x-Achse von 'Lotfusspunkt auf Ebene'",
                            "parameter" : [
                                {
                                    "name" : "RegisterPositionToPlane",
                                    "feature": "Point",
                                    "comment" : "Lotfusspunkt von Punkt auf Ebene",
                                    "parameter" : [
                                        { "name": "Position" },
                                        { "name": "Plane" }
                                    ]
                                }
                            ]
                        }
                    ]
                }

            ]
        },
        {
            "name": "YDistance",
            "description": "Project a point onto a plane along the y axis.",
            "applicableFor" : [ "ScalarEntityDistance" ],

            "neededElements": [
                {
                    "name": "Position",
                    "description" : "Select one point to register to the plane.",
                    "infinite": true,
                    "typeOfElement": "Position"
                },
                {
                    "name": "Plane",
                    "description" : "Select a plane in which the target geometry is to be projected.",
                    "infinite": false,
                    "typeOfElement": "Plane"
                }
            ],

            "parameter": [
                {
                    "name" : "DistanceBetweenTwoPoints",
                    "feature": "global",
                    "parameter" : [
                        {
                            "name" : "XZFilter",
                            "feature": "Point",
                            "comment" : "y-Achse",
                            "parameter" : [
                                { "name": "Position" }
                            ]
                        },
                        {
                            "name" : "XZFilter",
                            "feature": "Point",
                            "comment" : "y-Achse von 'Lotfusspunkt auf Ebene'",
                            "parameter" : [
                                {
                                    "name" : "RegisterPositionToPlane",
                                    "feature": "Point",
                                    "comment" : "Lotfusspunkt von Punkt auf Ebene",
                                    "parameter" : [
                                        { "name": "Position" },
                                        { "name": "Plane" }
                                    ]
                                }
                            ]
                        }
                    ]
                }

            ]
        },
        {
            "name": "ZDistance",
            "description": "Project a point onto a plane along the z axis.",
            "applicableFor" : [ "ScalarEntityDistance" ],

            "neededElements": [
                {
                    "name": "Position",
                    "description" : "Select one point to register to the plane.",
                    "infinite": true,
                    "typeOfElement": "Position"
                },
                {
                    "name": "Plane",
                    "description" : "Select a plane in which the target geometry is to be projected.",
                    "infinite": false,
                    "typeOfElement": "Plane"
                }
            ],

            "parameter": [
                {
                    "name" : "DistanceBetweenTwoPoints",
                    "feature": "global",
                    "parameter" : [
                        {
                            "name" : "XYFilter",
                            "feature": "Point",
                            "comment" : "z-Achse",
                            "parameter" : [
                                { "name": "Position" }
                            ]
                        },
                        {
                            "name" : "XYFilter",
                            "feature": "Point",
                            "comment" : "z-Achse von 'Lotfusspunkt auf Ebene'",
                            "parameter" : [
                                {
                                    "name" : "RegisterPositionToPlane",
                                    "feature": "Point",
                                    "comment" : "Lotfusspunkt von Punkt auf Ebene",
                                    "parameter" : [
                                        { "name": "Position" },
                                        { "name": "Plane" }
                                    ]
                                }
                            ]
                        }
                    ]
                }

            ]
        }
    ]
}

)"
