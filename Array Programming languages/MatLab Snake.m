function Snake
snake = struct('x',0,'y',15);
dir = 'right';
cur = snake;
food = snake;
MAX_RC = 30;
BLOCK_SIZE = 30;
hf = figure('Units','Pixels','Position',[500,1,1000,1000],'KeyPressFcn',@myKey);
axes(hf,'Units','Pixels','Position',[50,50,900,900],'XLim',[1,900],'YLim',[1,900],'NextPlot','add');
ht = timer('ExecutionMode','fixedRate','Period',0.1,'TimerFcn',@myTimer);
SetFood();
    function Print(pt,mode)
        switch mode
            case 'food'
                bColor = 'r';
            case 'body'
                bColor = 'k';
            case 'air'
                bColor = 'w';
        end
        rectangle('FaceColor',bColor,'EdgeColor','w','Position',BLOCK_SIZE*[pt.x,pt.y,1,1]);
    end
    function SetFood
        while 1
            food.x = randi([0,MAX_RC-1],1);
            food.y = randi([0,MAX_RC-1],1);
            if ~Travel(food)
                break;
            end
        end
        Print(food,'food');
    end
    function ret = Travel(pt)
        for i= 1:size(snake,2)
            if isequal(pt,snake(i))
                ret = 1;
                return;
            end
        end
        ret =0;
    end
    function myKey(~,e)
        switch e.Key
            case 's'
                start(ht);
            case 'uparrow'
                if strcmp(dir,'left') || strcmp(dir,'right')
                    dir = 'up';
                end
            case 'downarrow'
                if strcmp(dir,'left') || strcmp(dir,'right')
                    dir = 'down';
                end
            case 'leftarrow'
                if strcmp(dir,'up') || strcmp(dir,'down')
                    dir = 'left';
                end
            case 'rightarrow'
                if strcmp(dir,'up') || strcmp(dir,'down')
                    dir = 'right';
                end
        end
    end
    function myTimer(~,~)
        switch dir
            case 'up'
                cur.y = cur.y + 1;
                if cur.y > MAX_RC-1
                    cur.y = 0;
                end
            case 'down'
                cur.y = cur.y - 1;
                if cur.y < 0
                    cur.y = MAX_RC-1;
                end
            case 'left'
                cur.x = cur.x - 1;
                if cur.x < 0
                    cur.x = MAX_RC-1;
                end
            case 'right'
                cur.x = cur.x + 1;
                if cur.x > MAX_RC-1
                    cur.x = 0;
                end
        end
        if ~Travel(cur)
            snake(end+1) = cur;
            Print(cur,'body');
            if ~Travel(food)
                Print(snake(1),'air');
                snake = snake(2:end);
            else
               SetFood(); 
            end
        else
            error("Game Over!!!");
            stop(ht);
            delete(ht);
        end
    end
end
