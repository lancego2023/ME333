function client(port)
    % provides a menu for accessing PIC32 motor control functions 
    %
    % client(port) 
    %
    % Input Arguments: 
    % port - the name of the com port. This should be the same as what 
    % you use in screen or putty in quotes ’ ’ 
    %
    % Example: 
    % client(’/dev/ttyUSB0’) (Linux/Mac) 
    % client(’COM3’) (PC) 
    %
    % For convenience, you may want to change this so that the port is hardcoded. 
    % Opening COM connection 
    if  ~isempty(instrfind)
        fclose(instrfind);
        delete(instrfind);
    end
    fprintf('Opening port %s....\n',port);
    % settings for opening the serial port. baud rate 230400, hardware flow control 
    % wait up to 120 seconds for data before timing out 
    mySerial = serial(port, 'BaudRate', 230400, 'FlowControl', 'hardware','Timeout',120);
    % opens serial connection 
    fopen(mySerial);
    % closes serial port when function exits 
    clean = onCleanup(@()fclose(mySerial));
    has_quit = false;
    % menu loop 
    
    while  ~has_quit
        fprintf('\n       PIC32 MOTOR DRIVER INTERFACE\n\n');
        % display the menu options; this list will grow
        fprintf(' a: Read Current (counts)    b: Read Current (mA)\n');
        fprintf(' c: Read Encoder (counts)    d: Read Encoder (degrees)\n');
        fprintf(' e: Reset Encoder            f: PWM mode\n');
        fprintf(' g: Set Current Gain         h: Get Current Gain\n');
        fprintf(' i: Set Position Gain        j: Get Position Gain\n');
        fprintf(' k: Test Current Gains       l: Go to Angle (deg)\n');
        fprintf(' m: Load Step Trajectory     n: Load Cubic Trajectory\n');
        fprintf(' o: Execute Trajectory       p: power off    \n');
        fprintf(' r: Get Mode                 q: Quit\n');
        % read the user’s choice 
        selection = input('\nENTER COMMAND: ', 's');
        % send the command to the PIC32 
        fprintf(mySerial,'%c\n',selection);
        % take the appropriate action 
        switch selection
            case 'a' %read current (counts)
                icount = fscanf(mySerial, '%d');
                fprintf('The current is %d counts\n', icount);
            case 'b' %read current (mA)
                imA = fscanf(mySerial, '%f');
                fprintf('The current is %.2f mA\n', imA);
            case 'c' %read encoder pos (counts)
                counts = fscanf(mySerial, '%d');
                fprintf('The motor angle is %d counts\n',counts);
            case 'd' % read encoder pos (degrees)
                degrees = fscanf(mySerial, '%f');
                fprintf('The motor angle is %.2f degrees\n',degrees);
            case 'e' % reset encoder pos
                fprintf('Reseting encoder to 0...\n');
            case 'f'
                n = input('enter PWM percentage (-100 to 100): ');
                fprintf(mySerial, '%d\n', n);
            case 'g' % set current gains
                newP = input('Enter proportional gain: ');
                fprintf(mySerial, '%d\n', newP);
                newI = input('Enter integral gain: ');
                fprintf(mySerial, '%d\n', newI);
            case 'h' %get current gains
                gains = fscanf(mySerial);
                fprintf(gains);
            case 'i' % set position gains
                newP = input('Enter proportional gain: ');
                fprintf(mySerial, '%f\n', newP);
                newI = input('Enter integral gain: ');
                fprintf(mySerial, '%f\n', newI);
                newD = input('Enter derivative gain: ');
                fprintf(mySerial, '%f\n', newD);
            case 'j' %get position gains
                gains = fscanf(mySerial);
                fprintf(gains);
            case 'k'
                read_plot_matrix(mySerial);
            case 'l'
                angle = input('enter angle (-360 to 360): ');
                fprintf(mySerial, '%f\n', angle);
            case 'm' % step trajectory
                A = input('Input Step Trajectory: \n');
                stepref = genRef(A, 'step');
                fprintf(mySerial, '%d\n', length(stepref));
                for ii=1:length(stepref)
                    fprintf(mySerial, '%f\n', stepref(ii));
                end
            case 'n' % cubic trajectory
                B = input('Input Cubic Trajectory: \n');
                cubicref = genRef(B, 'cubic'); 
                fprintf(mySerial, '%d\n', length(cubicref));
                for ii=1:length(cubicref)
                    fprintf(mySerial, '%f\n', cubicref(ii));
                end
            case 'o'
                fprintf('Executing trajectory...\n');
                read_plot_matrix(mySerial);
            case 'p'
                fprintf('powering off motor... \n');
            case 'q'
                has_quit = true; % exit client
            case 'r' % prints the mode
                mode = fscanf(mySerial);
                fprintf(mode);
            otherwise
                fprintf('Invalid Selection %c\n', selection);
        end
    end
end