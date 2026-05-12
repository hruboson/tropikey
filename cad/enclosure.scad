/**
 * Enclosure for the TROPIC01 USB Devkit
 * All measurements in mm
 * @author Ondřej Hruboš (github.com/hruboson)
 */
$fn = 100; // fragment number - level of detail

// board dimensions
board_length = 45;
board_width = 20;
floor_thickness = 0.6;
max_component_height = 2.2; // tallest component/2
neck_width = 12;
neck_height = 11;
neck_start_x = board_length - neck_height;
board_to_neck_ratio_width = neck_width / board_width; // 60% ... maybe change the whole logic so I can adjust neck length
board_to_neck_ratio_height = neck_start_x / board_length;

// enclosure parameters
wall_thickness = 3;
clearance = 1.0;
total_height = floor_thickness + max_component_height + clearance;
clip_height = 3;
clip_undercut = 0.8;
half_height = total_height/2 + 1;

// alignment pin
pin_radius = 1;
pin_depth = half_height;

// helper constants
RERR = 0.01; // rounding error

/**
 * Construct one half of the enclosure
 * @param {bool} [upper=false] - upper or lower case, changes the joints
 * @param {real} l - length
 * @param {real} w - width
 * @param {real} h - height
 * @param {bool} [base=true] - false to not generate any joints
 */
module enclosure_half(upper = false, l = undef, w = undef, h = undef, base = true) {
    length = (l == undef) ? board_length + 2*wall_thickness : l;
    width = (l == undef) ? board_width + 2*wall_thickness : w;
    height = (l == undef) ? half_height : h;
    
    difference() {
        rounded_rect(
            length,
            width,
            height,
            radius = 2
        );
        
        // cutoff from sides
        translate([length*board_to_neck_ratio_height + RERR, -RERR, -RERR])
        cube([
            length*(1-board_to_neck_ratio_height) + RERR,
            width*(1-board_to_neck_ratio_width)/2 + RERR,
            height + RERR*2
        ]);
        translate([
                length*board_to_neck_ratio_height + RERR, 
                width-width*(1-board_to_neck_ratio_width)/2 + RERR, 
                -RERR
            ])
        cube([
            length*(1-board_to_neck_ratio_height) + RERR,
            width*(1-board_to_neck_ratio_width)/2 + RERR,
            height + RERR*2
        ]);
        
        // cutoff at the neck top
        translate([board_length + wall_thickness - RERR, 0, -0.1])
        cube([wall_thickness + 2*RERR, width, height+3]);
        
        // dimple joints
        if(!upper && base){        
            translate([wall_thickness*0.7, wall_thickness*0.7, height])
            cylinder(h = pin_depth, r = pin_radius, center = true);
            
            translate([wall_thickness*0.7, width-wall_thickness*0.7, height])        
            cylinder(h = pin_depth, r = pin_radius, center = true);
            
            translate([length*0.7, wall_thickness*0.5, height])
            cylinder(h = pin_depth, r = pin_radius, center = true);

            translate([length*0.7, width-wall_thickness*0.5, height])
            cylinder(h = pin_depth, r = pin_radius, center = true);
            
            translate([length*0.4, wall_thickness*0.5, height])
            cylinder(h = pin_depth, r = pin_radius, center = true);
            
            translate([length*0.4, width-wall_thickness*0.5, height])
            cylinder(h = pin_depth, r = pin_radius, center = true);
        }
        
        // hole for button
        if(upper && base){
            translate([length*0.7,width/2, -RERR])
            cube([6, width*0.3, half_height], center = true);
        }
    }
    
    // upper half
    if(upper && base){
        pin_outwards_radius = pin_radius - 0.15; // this might need adjusting
        
        translate([wall_thickness*0.7, wall_thickness*0.7, height])
        cylinder(h = pin_depth, r = pin_outwards_radius, center = true);
        
        translate([wall_thickness*0.7, width-wall_thickness*0.7, height])        
        cylinder(h = pin_depth, r = pin_outwards_radius, center = true);
        
        translate([length*0.7, wall_thickness*0.5, height])
        cylinder(h = pin_depth, r = pin_outwards_radius, center = true);

        translate([length*0.7, width-wall_thickness*0.5, height])
        cylinder(h = pin_depth, r = pin_outwards_radius, center = true);
        
        translate([length*0.4, wall_thickness*0.5, height])
        cylinder(h = pin_depth, r = pin_outwards_radius, center = true);
        
        translate([length*0.4, width-wall_thickness*0.5, height])
        cylinder(h = pin_depth, r = pin_outwards_radius, center = true);
    }
}

/**
 * Create a rectangle with rounded corners and uniform height
 * @param {real} length - length of the rectangle along X-axis
 * @param {real} width - width of the rectangle along Y-axis
 * @param {real} height - height of the extruded shape along Z-axis
 * @param {real} [radius=2] - corner radius in mm
 */
module rounded_rect(length, width, height, radius=2) {
    hull() {
        translate([radius, radius, 0])
            cylinder(r=radius, h=height);
        translate([length-radius, radius, 0])
            cylinder(r=radius, h=height);
        translate([radius, width-radius, 0])
            cylinder(r=radius, h=height);
        translate([length-radius, width-radius, 0])
            cylinder(r=radius, h=height);
    }
}

//////////////////////
//      MAIN        //
//////////////////////

translate([-board_length/2 - wall_thickness, -board_width/2 - wall_thickness, 0]) {
    difference() {
        enclosure_half(l = board_length + 2*wall_thickness, w = board_width + 2*wall_thickness, h = half_height);
        
        translate([wall_thickness, wall_thickness, 1])
        enclosure_half(upper = false, l = board_length, w = board_width, h = half_height, base = false);
    }
}

translate([-board_length/2 - wall_thickness + 60, -board_width/2 - wall_thickness, 0]) {
    difference() {
        enclosure_half(upper = true, l = board_length + 2*wall_thickness, w = board_width + 2*wall_thickness, h = half_height);
        
        translate([wall_thickness, wall_thickness, 1])
        enclosure_half(upper = true, l = board_length, w = board_width, h = half_height, base = false);
    }
}