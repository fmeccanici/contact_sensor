#include "ContactPlugin.hh"

using namespace gazebo;
GZ_REGISTER_SENSOR_PLUGIN(ContactPlugin)

/////////////////////////////////////////////////
ContactPlugin::ContactPlugin() : SensorPlugin()
{
}

/////////////////////////////////////////////////
ContactPlugin::~ContactPlugin()
{
}

/////////////////////////////////////////////////
void ContactPlugin::Load(sensors::SensorPtr _sensor, sdf::ElementPtr /*_sdf*/)
{
  // Get the parent sensor.
  this->parentSensor =
    boost::dynamic_pointer_cast<sensors::ContactSensor>(_sensor);

  // Make sure the parent sensor is valid.
  if (!this->parentSensor)
  {
    gzerr << "ContactPlugin requires a ContactSensor.\n";
    return;
  }

  // Connect to the sensor update event.
  this->updateConnection = this->parentSensor->ConnectUpdated(
      boost::bind(&ContactPlugin::OnUpdate, this));

  // Make sure the parent sensor is active.
  this->parentSensor->SetActive(true);

  this->gripper_collision_pub = nh.advertise<contact_sensor::GripperCollision>("gripper_collision", 10);
}

bool ContactPlugin::isGripperColliding()
{
  msgs::Contacts contacts;
  contacts = this->parentSensor->GetContacts();
  for (int i = 0; i < contacts.contact_size(); ++i)
  {
    if (contacts.contact(i).collision1().substr(0,5) == "marco" || contacts.contact(i).collision2().substr(0,5) == "marco")
    {   
      for (int j = 0; j < contacts.contact(i).position_size(); j++)
        {
          this->x = contacts.contact(i).position(j).x();
          this->y = contacts.contact(i).position(j).y();
          this->z = contacts.contact(i).position(j).z();
        }
        return true;
    }  
  }
  return false;
}

// void ContactPlugin::setCollision()
// {
//   msgs::Contacts contacts;
//   contacts = this->parentSensor->GetContacts();
//   for (int i = 0; i < contacts.contact_size(); ++i)
//   {
//     std::cout << contacts.contact(i).collision1().substr(0,5) << std::endl;
//     std::cout << contacts.contact(i).collision2().substr(0,5) << std::endl;

//     if (contacts.contact(i).collision1().substr(0,5) == "marco" || contacts.contact(i).collision2().substr(0,5) == "marco")
//     {   
//       for (int j = 0; j < contacts.contact(i).position_size(); j++)
//         {
//           this->msg.position.x = contacts.contact(i).position(j).x();
//           this->msg.position.y = contacts.contact(i).position(j).y();
//           this->msg.position.z = contacts.contact(i).position(j).z();
//           this->msg.collision.data = true;
//         }
//     }  
//     else if (contacts.contact(i).collision1().substr(0,5) == "dishw" || contacts.contact(i).collision2().substr(0,5) == "dishw")
// 	  {
// 	  this->msg.position.x = 0;
// 	  this->msg.position.y = 0;
// 	  this->msg.position.z = 0;
// 	  this->msg.collision.data = false;
//     }

// }
  



/////////////////////////////////////////////////
void ContactPlugin::OnUpdate()
{
  // Get all the contacts.
  msgs::Contacts contacts;
  contacts = this->parentSensor->GetContacts();
  // this->setCollision();
  
  if (isGripperColliding())
  {
    this->msg.position.x = this->x;
    this->msg.position.y = this->y;
    this->msg.position.z = this->z;
    this->msg.collision.data = true;
  }
  else
  {
    this->msg.position.x = 0;
    this->msg.position.y = 0;
    this->msg.position.z = 0;
    this->msg.collision.data = false;
  }

  this->gripper_collision_pub.publish(this->msg);
  
}

