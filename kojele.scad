module screw(l=16,d=3,fit=1.2,res=20){
	translate([0,0,-l/2])
	cylinder(h=l,r=d/2*fit,center=true,$fn=res);
	//head
	translate([0,0,-1.65/2])
	cylinder(h=1.65,r1=d/2*fit,r2=d*1.9/2*fit,center=true,$fn=res);
}

module PDS4(l = 32) {
    difference(){
        cube([16,l,12]);
        translate([2.5,-1,1])
        cube([11,l+2,12]);

        for (i = [1:2:5]) {
            translate([-0.5, -1, i])
            cube([2, l+2, 1]);
            translate([16 - 1.5, -1, i])
            cube([2, l+2, 1]);
        }

        translate([1.5, l+1, 8])
        rotate([90, 0, 0])
        linear_extrude(height = l+2)
            polygon([[1.01, 4], [0, 0], [1.01, 0]]);

        translate([14.5, l+1, 8])
        mirror()
        rotate([90, 0, 0])
        linear_extrude(height = l+2)
            polygon([[1.01, 4], [0, 0], [1.01, 0]]);

    }
}

module foot() {
    translate([1,1,1]) {
    difference() {
        color("dimgray") minkowski(){
            difference(){
                cube([20,30,20]);

                translate([0, 0, 20])
                    rotate(a=[0,45,0]) {
                        cube([16,32,16]);
                }

            }
            sphere(1, $fn=40);
        }

*        color("darkgrey") translate([0, -1, 21.3])
            rotate(a=[0,45,0]) {
                translate([8,16,-15]){
                    cylinder(25, 1.6, 1.6, $fn=50);
                }	
                translate([8,16,-30]){
                    rotate([0, 0, 30])
                    cylinder(25, 3.1, 3.1, $fn=6);
                }
        }
    }
    }
}

*for(i = [0: 25: 75]){
    for(j = [0: 35: 35]){
        translate([i, j, 0])
        foot();
    }
}

*translate([1,1,1])
translate([0, -1, 21.3])
    rotate(a=[0,45,0]) {
        color("silver") translate([0, -10, 0])PDS4(120);
        translate([8,16,1.1]){
             color("lightgrey") screw(10, fit=1, res=50);
        }	
        color("silver")
        translate([8,16,-7.5]){
            rotate([0, 0, 30])
            cylinder(2.4, 3, 3, $fn=6);
        }
}

foot();

