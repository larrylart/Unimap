/****************************************
 * 2D Spline Class
 * By Bill Perone (billperone@yahoo.com)
 * Original: 21-10-2003
 *   
 * Dependancies: matrix4, vector2 class, STL vector, STL list
 *
 * This code is provided "as is" and you can use it freely as long as 
 * credit is given to Bill Perone in the application it is used in
 ****************************************/


#include "Spline.h"




std::list<Spline1D::ArclenTableEntry>::iterator Spline1D::InternalBuildArclenTable(std::list<Spline1D::ArclenTableEntry> &templist,
                                                                                   std::list<Spline1D::ArclenTableEntry>::iterator curr, unsigned int ctrlpt_offset)
{
    std::list<ArclenTableEntry>::iterator next= curr;
    ArclenTableEntry  middle;


    const unsigned int size= ctrlpts.size();
    ++next;
    middle.time= (curr->time + (next)->time)/2;
    middle.pos= InterpCtrlPt(ctrlpts[ctrlpt_offset], ctrlpts[(ctrlpt_offset+1)%size],
                             ctrlpts[(ctrlpt_offset+2)%size], ctrlpts[(ctrlpt_offset+3)%size],
                             middle.time - (float)ctrlpt_offset/step);

    float a, b, c;
    a= middle.pos-curr->pos;
    b= next->pos-middle.pos;
    c= next->pos-curr->pos;

    float error= a+b-c;
    if (error > ARCLEN_GENERATION_ERROR)
    {
        // recalculate the arclengths for the rest of the table
        std::list<ArclenTableEntry>::iterator i= next;
        while (i != templist.end())
        {
            i->arclen+=error;
            ++i;
        }

        // insert the mid point into the table
        middle.arclen= curr->arclen+a;
        templist.insert(next, middle);

        // recursive subdivision
        next= InternalBuildArclenTable(templist, curr, ctrlpt_offset);
        next= InternalBuildArclenTable(templist, next, ctrlpt_offset);
    }
    return next;
}


void Spline1D::ArclenTableBuild()
{
    std::list<ArclenTableEntry> templist;
    std::list<ArclenTableEntry>::iterator ai;

    ArclenTableEntry   entry;
    unsigned int size= ctrlpts.size();


    arclentable.clear();

    // add the first entry to the arc table
    entry.time= 0;
    entry.arclen= 0;
    entry.pos= InterpCtrlPt(ctrlpts[0], ctrlpts[1], ctrlpts[2%size], ctrlpts[3%size], 0);
    templist.push_back(entry);

    // go through all the control point sets generating arclen table entries
    ai= templist.begin();

    for (unsigned int i= 0; i < ctrlpts.size()-(step-1); i+=step)
    {
        // add the end entry to the list
        entry.time= (float)(i+step)/step;
        entry.pos= InterpCtrlPt(ctrlpts[i%size], ctrlpts[(i+1)%size], ctrlpts[(i+2)%size], ctrlpts[(i+3)%size], 1);
        entry.arclen= entry.pos-ai->pos + templist.back().arclen; // beginning approximation
        templist.push_back(entry);

        // do the subdivision
        ai= InternalBuildArclenTable(templist, ai, i);
    }

    ai= templist.begin();
    while (ai!=templist.end())
    {
        ai->time/=templist.back().time;
        arclentable.push_back(*ai);
        ++ai;
    }
}


float Spline1D::ArcPoint(float arc_val)
{
    // trivial cases
    if (arc_val==arclentable.front().arclen) return arclentable.front().pos;
    else
    if (arc_val==arclentable.back().arclen) return arclentable.back().pos;

    unsigned int lower= ArcTableBisectionSearch(arc_val);

    // perform a linear interpolation of 2 nearest position values
    float pos;
    pos= ( (arclentable[lower+1].arclen-arc_val)*arclentable[lower].pos +
           (arc_val-arclentable[lower].arclen)*arclentable[lower+1].pos ) /
           (arclentable[lower+1].arclen - arclentable[lower].arclen);
    return pos;
}


std::list<Spline2D::ArclenTableEntry>::iterator Spline2D::InternalBuildArclenTable(std::list<Spline2D::ArclenTableEntry> &templist,
                                                                                   std::list<Spline2D::ArclenTableEntry>::iterator curr, unsigned int ctrlpt_offset)
{
    std::list<ArclenTableEntry>::iterator next= curr;
    ArclenTableEntry  middle;


    const unsigned int size= ctrlpts.size();
    ++next;
    middle.time= (curr->time + (next)->time)/2;
    middle.pos.x= InterpCtrlPt(ctrlpts[ctrlpt_offset].x, ctrlpts[(ctrlpt_offset+1)%size].x,
                               ctrlpts[(ctrlpt_offset+2)%size].x, ctrlpts[(ctrlpt_offset+3)%size].x,
                               middle.time - (float)ctrlpt_offset/step);
    middle.pos.y= InterpCtrlPt(ctrlpts[ctrlpt_offset].y, ctrlpts[(ctrlpt_offset+1)%size].y,
                               ctrlpts[(ctrlpt_offset+2)%size].y, ctrlpts[(ctrlpt_offset+3)%size].y,
                               middle.time - (float)ctrlpt_offset/step);

    float a, b, c;
    a= vector2f(middle.pos-curr->pos).length();
    b= vector2f(next->pos-middle.pos).length();
    c= vector2f(next->pos-curr->pos).length();

    float error= a+b-c;
    if (error > ARCLEN_GENERATION_ERROR)
    {
        // recalculate the arclengths for the rest of the table
        std::list<ArclenTableEntry>::iterator i= next;
        while (i != templist.end())
        {
            i->arclen+=error;
            ++i;
        }

        // insert the mid point into the table
        middle.arclen= curr->arclen+a;
        templist.insert(next, middle);

        // recursive subdivision
        next= InternalBuildArclenTable(templist, curr, ctrlpt_offset);
        next= InternalBuildArclenTable(templist, next, ctrlpt_offset);
    }
    return next;
}


void Spline2D::ArclenTableBuild()
{
    std::list<ArclenTableEntry> templist;
    std::list<ArclenTableEntry>::iterator ai;

    ArclenTableEntry   entry;
    unsigned int size= ctrlpts.size();


    arclentable.clear();

    // add the first entry to the arc table
    entry.time= 0;
    entry.arclen= 0;
    entry.pos.x= InterpCtrlPt(ctrlpts[0].x, ctrlpts[1].x, ctrlpts[2%size].x, ctrlpts[3%size].x, 0);
    entry.pos.y= InterpCtrlPt(ctrlpts[0].y, ctrlpts[1].y, ctrlpts[2%size].y, ctrlpts[3%size].y, 0);
    templist.push_back(entry);

    // go through all the control point sets generating arclen table entries
    ai= templist.begin();

    for (unsigned int i= 0; i < ctrlpts.size()-(step-1); i+=step)
    {
        // add the end entry to the list
        entry.time= (float)(i+step)/step;
        entry.pos.x= InterpCtrlPt(ctrlpts[i%size].x, ctrlpts[(i+1)%size].x, ctrlpts[(i+2)%size].x, ctrlpts[(i+3)%size].x, 1);
        entry.pos.y= InterpCtrlPt(ctrlpts[i%size].y, ctrlpts[(i+1)%size].y, ctrlpts[(i+2)%size].y, ctrlpts[(i+3)%size].y, 1);
        entry.arclen= vector2f(entry.pos-ai->pos).length() + templist.back().arclen; // beginning approximation
        templist.push_back(entry);

        // do the subdivision
        ai= InternalBuildArclenTable(templist, ai, i);
    }

    ai= templist.begin();
    while (ai!=templist.end())
    {
        ai->time/=templist.back().time;
        arclentable.push_back(*ai);
        ++ai;
    }
}


vector2f Spline2D::ArcPoint(float arc_val)
{
    // trivial cases
    if (arc_val==arclentable.front().arclen) return arclentable.front().pos;
    else
    if (arc_val==arclentable.back().arclen) return arclentable.back().pos;

    unsigned int lower= ArcTableBisectionSearch(arc_val);

    // perform a linear interpolation of 2 nearest position values
    vector2f pos;
    pos.x= ( (arclentable[lower+1].arclen-arc_val)*arclentable[lower].pos.x +
             (arc_val-arclentable[lower].arclen)*arclentable[lower+1].pos.x ) /
             (arclentable[lower+1].arclen - arclentable[lower].arclen);
    pos.y= ( (arclentable[lower+1].arclen-arc_val)*arclentable[lower].pos.y +
             (arc_val-arclentable[lower].arclen)*arclentable[lower+1].pos.y ) /
             (arclentable[lower+1].arclen - arclentable[lower].arclen);

    return( pos );
}
